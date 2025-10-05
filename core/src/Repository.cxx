/**
 * @file Repository.cxx
 * @brief Implementation of the Repository class, which serves as the main application facade and core logic for the VCS.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/Repository.hxx"
#include "../include/Blob.hxx"
#include "../../services/IObserver.hxx"
#include "../../services/Event.hxx"
#include "../include/Utils.hxx"

#include <algorithm>
#include <iostream>
#include <fstream>
#include <utility>

namespace fs = std::filesystem;

Repository::~Repository() {
    std::cout << "DEBUG: Repository destroyed for: " << root_path.string() << std::endl;
}


Repository::Repository(std::filesystem::path  rpath)
    : observers(), 
      root_path(std::move(rpath))
{
    objects = std::make_unique<ObjectStorage>(root_path.string(), nullptr); 
}

void Repository::initializeDependencies() {
    auto repository_ptr = std::make_shared<Repository>(root_path);
    
}


bool Repository::init() {
    const std::string SOURCE = "Repository";
    
    // 1. Определение всех необходимых путей
    fs::path svcs_path = root_path / ".svcs";
    fs::path refs_path = svcs_path / "refs";
    fs::path heads_path = refs_path / "heads";  // ← ДОБАВЛЯЕМ ЭТУ ДИРЕКТОРИЮ
    fs::path objects_path = svcs_path / "objects";
    fs::path head_file_path = svcs_path / "HEAD";

    try {
        // ИСПРАВЛЕНИЕ: Если репозиторий уже инициализирован, возвращаем true
        if (is_initialized()) {
            notify({Event::GENERAL_INFO, "Repository already exists at " + svcs_path.string(), SOURCE});
            return true;
        }

        // 2. Создание всех основных подкаталогов: .svcs, objects, refs, refs/heads
        if (!fs::exists(svcs_path)) {
            if (!fs::create_directory(svcs_path)) {
                notify({Event::FATAL_ERROR, "Failed to create .svcs directory at: " + svcs_path.string(), SOURCE});
                return false;
            }
            notify({Event::GENERAL_INFO, "Created .svcs directory.", SOURCE});
        }

        if (!fs::exists(objects_path)) {
            fs::create_directories(objects_path);
            notify({Event::GENERAL_INFO, "Created objects directory.", SOURCE});
        }

        if (!fs::exists(refs_path)) {
            fs::create_directories(refs_path);
            notify({Event::GENERAL_INFO, "Created refs directory.", SOURCE});
        }

        // ДОБАВЛЯЕМ: Создание директории refs/heads
        if (!fs::exists(heads_path)) {
            fs::create_directories(heads_path);
            notify({Event::GENERAL_INFO, "Created refs/heads directory.", SOURCE});
        }
        
        // 3. Создание служебных файлов (HEAD)
        if (!fs::exists(head_file_path)) {
            std::ofstream head_file(head_file_path);
            if (head_file.is_open()) {
                head_file << "ref: refs/heads/main\n"; 
                head_file.close();
                notify({Event::GENERAL_INFO, "Created initial HEAD file.", SOURCE});
            } else {
                notify({Event::FATAL_ERROR, "Failed to create HEAD file.", SOURCE});
                return false;
            }
        }

        // 4. Успешное завершение
        notify({Event::GENERAL_INFO, "Repository initialization completed successfully", SOURCE});
        return true;
        
    } catch (const fs::filesystem_error& e) {
        std::cerr << "!!! FATAL FILESYSTEM ERROR !!!: " << e.what() << std::endl; 
        
        notify({
            Event::FATAL_ERROR,
            "Failed to initialize repository structure in " + root_path.string() + ": " + std::string(e.what()),
            SOURCE
        });
        return false;
    }
}

bool Repository::is_initialized() const {
    auto svcs_dir = root_path / ".svcs";
    auto objects_dir = svcs_dir / "objects";
    auto refs_dir = svcs_dir / "refs";
    auto heads_dir = refs_dir / "heads";
    auto head_file = svcs_dir / "HEAD";
    
    return fs::exists(svcs_dir) && 
           fs::exists(objects_dir) &&
           fs::exists(refs_dir) &&
           fs::exists(heads_dir) &&
           fs::exists(head_file);
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
    while (true) {
        fs::path svcs_dir = current_path / ".svcs";
        
        // Проверяем существует ли директория .svcs
        if (fs::exists(svcs_dir) && fs::is_directory(svcs_dir)) {
            try {
                auto repo = std::make_shared<Repository>(current_path.string());
                if (repo->is_initialized()) {
                    return repo;
                }
            } catch (const std::exception& e) {
                std::cerr << "Warning: Found .svcs but failed to create repository: " 
                          << e.what() << std::endl;
            }
        }
        
        if (current_path == current_path.parent_path()) {
            break;
        }
        
        current_path = current_path.parent_path();
    }
    
    return nullptr;
}

bool Repository::isRepository(const std::string& path) {
    fs::path repo_path(path);
    fs::path svcs_dir = repo_path / ".svcs";
    
    if (!fs::exists(svcs_dir) || !fs::is_directory(svcs_dir)) {
        return false;
    }
    
    fs::path objects_dir = svcs_dir / "objects";
    fs::path refs_dir = svcs_dir / "refs";
    
    return fs::exists(objects_dir) && fs::is_directory(objects_dir) &&
           fs::exists(refs_dir) && fs::is_directory(refs_dir);
}

bool Repository::stageFile(const std::string& path) {
    const std::string SOURCE = "Repository";
    fs::path absolutePath = root_path / path;

    if (!fs::exists(absolutePath)) {
        notify({Event::RUNTIME_ERROR, "File not found at path: " + path, SOURCE});
        return false;
    }
    
    if (!index) {
        notify({Event::RUNTIME_ERROR, "Internal error: Index manager is not initialized.", SOURCE});
        return false;
    }

    notify({Event::DEBUG_MESSAGE, "Attempting to stage file: " + path, SOURCE});

    std::string content;
    try {
        content = read_file_to_string(absolutePath);
    } catch (const std::runtime_error& e) {
        notify({Event::RUNTIME_ERROR, "Failed to read file: " + path + " (" + e.what() + ")", SOURCE});
        return false;
    }

    std::string objectHash = Blob::calculateHash(content); 

    try {
        ObjectStorage* objStore = getObjectStorage();
        const Blob blob = {content};
        objStore->saveObject(blob);
    } catch (const std::exception& e) {
        notify({Event::RUNTIME_ERROR, "Failed to store object for " + path + ": " + e.what(), SOURCE});
        return false;
    }

    try {
        [[maybe_unused]] unsigned int fileMode = static_cast<unsigned int>(fs::status(absolutePath).permissions());


        const IndexEntry ie = {objectHash, path};
        index->addEntry(ie); 
        
        if (!index->write()) {
             notify({Event::RUNTIME_ERROR, "Failed to write index file to disk.", SOURCE});
             return false;
        }

        notify({Event::GENERAL_INFO, "File " + path + " successfully added to index.", SOURCE});
        return true;
        
    } catch (const std::exception& e) {
        notify({Event::RUNTIME_ERROR, "Failed to manage index file for " + path + ": " + e.what(), SOURCE});
        return false;
    }
}
