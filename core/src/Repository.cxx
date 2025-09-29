#include "../include/Repository.hxx"
#include "../../services/IObserver.hxx"
#include "../../services/Event.hxx"
#include <algorithm>

namespace fs = std::filesystem;

Repository::Repository(const std::string& rpath) 
    : observers(), 
      root_path(rpath)
{
    objects = std::make_unique<ObjectStorage>(root_path, nullptr); 
}

void Repository::initializeDependencies() {
    objects->setSubject(this);
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

void Repository::attach(IObserver* observer) {
    if (observer) {
        observers.push_back(observer);
        Event e;
        e.type = Event::GENERAL_INFO;
        e.details = "Observer attached successfully.";
        notify(e);
    }
}

void Repository::detach(IObserver* observer) {
    auto it = std::remove(observers.begin(), observers.end(), observer);
    if (it != observers.end()) {
        observers.erase(it, observers.end());
        Event e;
        e.type = Event::GENERAL_INFO;
        e.details = "Observer detached successfully.";
        notify(e);
    }
}

void Repository::notify(const Event& event) {
    for (IObserver* observer : observers) {
        if (observer != nullptr) {
            observer->update(event);
        }
    }
}
