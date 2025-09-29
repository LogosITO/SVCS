#include "../include/Repository.hxx"
#include "../../services/IObserver.hxx"
#include "../../services/Event.hxx"
#include <algorithm>
#include <iostream>

namespace fs = std::filesystem;

Repository::~Repository() {
    std::cout << "DEBUG: Repository destroyed for: " << root_path.string() << std::endl;
}


Repository::Repository(const std::string& rpath) 
    : observers(), 
      root_path(rpath)
{
    objects = std::make_unique<ObjectStorage>(root_path.string(), nullptr); 
}

void Repository::initializeDependencies() {
    auto repository_ptr = std::make_shared<Repository>(root_path);
    
}


bool Repository::init() {
    fs::path svcs_path = root_path / ".svcs";
    fs::path refs_path = svcs_path / "refs";
    fs::path objects_path = svcs_path / "objects";
    
    try {
        if (!fs::exists(svcs_path)) {
            fs::create_directory(svcs_path);
            Event e;
            e.type = Event::GENERAL_INFO;
            e.details = "Created .svcs directory.";
            notify(e);
        }

        if (!fs::exists(refs_path)) {
            fs::create_directories(refs_path);
            Event e;
            e.type = Event::GENERAL_INFO;
            e.details = "Created refs directory.";
            notify(e);
        }

        if (!fs::exists(objects_path)) {
            fs::create_directories(objects_path);
            Event e;
            e.type = Event::GENERAL_INFO;
            e.details = "Created objects directory.";
            notify(e);
        }

        return true;
    } catch (const fs::filesystem_error& e) {
        Event error_e;
        error_e.type = Event::FATAL_ERROR;
        error_e.details = "Failed to initialize repository structure: " + std::string(e.what());
        notify(error_e);
        return false;
    }
}

bool Repository::is_initialized() const {
    auto svcs_dir = root_path / ".svcs";
    return fs::exists(svcs_dir) && 
            fs::exists(svcs_dir / "objects") &&
            fs::exists(svcs_dir / "refs");
}


void Repository::attach(std::shared_ptr<IObserver> observer) {
    if (observer) {
        observers.push_back(observer); 
        Event e;
        e.type = Event::GENERAL_INFO;
        e.details = "Observer attached successfully.";
        notify(e);
    }
}


void Repository::detach(std::shared_ptr<IObserver> observer_to_remove) {
    
    auto it = std::remove_if(observers.begin(), observers.end(), 
        [&](const std::weak_ptr<IObserver>& weak_obs) {
            auto shared_obs = weak_obs.lock();
            return shared_obs == observer_to_remove;
        }
    );
    observers.erase(it, observers.end());
}


void Repository::notify(const Event& event) const {
    for (auto const& weak_observer : observers) {
        if (auto shared_observer = weak_observer.lock()) {
            shared_observer->update(event);
            
        } else {
            //pass
        }
    }
}



std::shared_ptr<Repository> Repository::findRepository(const std::string& start_path) {
    fs::path current_path(start_path);
    
    // Поднимаемся вверх по директориям пока не найдем .svcs
    while (true) {
        fs::path svcs_dir = current_path / ".svcs";
        
        // Проверяем существует ли директория .svcs
        if (fs::exists(svcs_dir) && fs::is_directory(svcs_dir)) {
            // Нашли репозиторий - создаем и возвращаем объект
            try {
                auto repo = std::make_shared<Repository>(current_path.string());
                // Проверяем что репозиторий правильно инициализирован
                if (repo->is_initialized()) {
                    return repo;
                }
            } catch (const std::exception& e) {
                // Если не удалось создать репозиторий, продолжаем поиск
                std::cerr << "Warning: Found .svcs but failed to create repository: " 
                          << e.what() << std::endl;
            }
        }
        
        // Если достигли корня файловой системы - прекращаем поиск
        if (current_path == current_path.parent_path()) {
            break;
        }
        
        // Поднимаемся на уровень выше
        current_path = current_path.parent_path();
    }
    
    // Репозиторий не найден
    return nullptr;
}

// Метод для проверки является ли путь репозиторием
bool Repository::isRepository(const std::string& path) {
    fs::path repo_path(path);
    fs::path svcs_dir = repo_path / ".svcs";
    
    // Проверяем базовую структуру репозитория
    if (!fs::exists(svcs_dir) || !fs::is_directory(svcs_dir)) {
        return false;
    }
    
    // Проверяем необходимые поддиректории
    fs::path objects_dir = svcs_dir / "objects";
    fs::path refs_dir = svcs_dir / "refs";
    
    return fs::exists(objects_dir) && fs::is_directory(objects_dir) &&
           fs::exists(refs_dir) && fs::is_directory(refs_dir);
}