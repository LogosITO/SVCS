/**
 * @file RemoteProtocol.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the RemoteProtocol class for remote communication protocol.
 *
 * @russian
 * @brief Реализация класса RemoteProtocol для протокола удаленной связи.
 */

#include "../include/RemoteProtocol.hxx"
#include "../../services/Event.hxx"

#include <sstream>
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

RemoteProtocol::RemoteProtocol(std::shared_ptr<ISubject> event_bus,
                               std::shared_ptr<RepositoryManager> repository_manager)
    : event_bus_(std::move(event_bus))
    , repository_manager_(std::move(repository_manager))
{
}

bool RemoteProtocol::handleReceivePack() {
    notifyProtocolStart("Starting receive-pack protocol (PUSH)");

    if (!processPushNegotiation()) {
        notifyProtocolError("Push negotiation failed");
        return false;
    }

    notifyProtocolSuccess("Receive-pack protocol completed successfully");
    return true;
}

bool RemoteProtocol::handleUploadPack() {
    notifyProtocolStart("Starting upload-pack protocol (PULL)");

    if (!processPullNegotiation()) {
        notifyProtocolError("Pull negotiation failed");
        return false;
    }

    notifyProtocolSuccess("Upload-pack protocol completed successfully");
    return true;
}

bool RemoteProtocol::processPushNegotiation() {
    notifyNegotiation("Starting push negotiation");

    std::unordered_set<std::string> wanted_objects;
    std::vector<std::pair<std::string, std::string>> ref_updates;
    std::string line;

    while (readLine(line)) {
        notifyNetworkReceive("Client: " + line);
        if (line == "DONE") {
            break;
        }

        if (line.rfind("WANT ", 0) == 0) {
            std::string object_hash = line.substr(5);
            if (isValidObjectHash(object_hash)) {
                wanted_objects.insert(object_hash);
                sendLine("ACK " + object_hash);
            } else {
                sendLine("NAK " + object_hash);
            }
        }
        else if (line.rfind("UPDATE ", 0) == 0) {
            std::istringstream iss(line.substr(7));
            std::string ref_name, new_hash;
            iss >> ref_name >> new_hash;

            if (isValidReference(ref_name) && isValidObjectHash(new_hash)) {
                ref_updates.emplace_back(ref_name, new_hash);
                sendLine("OK " + ref_name);
            } else {
                sendLine("ERROR Invalid reference or object hash");
            }
        }
        else if (line == "BEGIN_OBJECTS") {
            break;
        }
        else {
            sendLine("ERROR Unknown command: " + line);
            return false;
        }
    }

    if (!receiveObjects(wanted_objects)) {
        return false;
    }

    if (!updateReferences(ref_updates)) {
        return false;
    }

    sendLine("SUCCESS Push completed");
    return true;
}

bool RemoteProtocol::processPullNegotiation() {
    notifyNegotiation("Starting pull negotiation");

    std::unordered_set<std::string> client_has;
    std::string line;

    while (readLine(line)) {
        notifyNetworkReceive("Client: " + line);

        if (line == "DONE_HAS") {
            break;
        }

        if (line.rfind("HAS ", 0) == 0) {
            std::string object_hash = line.substr(4);
            if (isValidObjectHash(object_hash)) {
                client_has.insert(object_hash);
                sendLine("ACK " + object_hash);
            }
        }
    }

    auto missing_objects = findMissingObjects(client_has);
    notifyObjectTransfer("Sending " + std::to_string(missing_objects.size()) + " missing objects");

    sendLine("OBJECTS_COUNT " + std::to_string(missing_objects.size()));

    if (!sendObjects(missing_objects)) {
        return false;
    }

    sendLine("SUCCESS Pull completed");
    return true;
}

bool RemoteProtocol::receiveObjects(const std::unordered_set<std::string>& wanted_objects) {
    notifyObjectTransfer("Receiving " + std::to_string(wanted_objects.size()) + " objects");

    std::string line;
    size_t received_count = 0;

    while (readLine(line)) {
        if (line == "END_OBJECTS") {
            break;
        }

        if (line.rfind("OBJECT ", 0) == 0) {
            std::string object_hash = line.substr(7);

            std::string object_data;
            if (!readLine(object_data)) {
                sendProtocolError("Failed to read object data");
                return false;
            }

            if (wanted_objects.count(object_hash)) {
                if (writeObject(object_hash, object_data)) {
                    received_count++;
                    sendLine("ACK " + object_hash);
                } else {
                    sendLine("NAK " + object_hash);
                }
            } else {
                sendLine("NAK " + object_hash + " (not requested)");
            }
        }
    }

    notifyObjectTransfer("Successfully received " + std::to_string(received_count) + " objects");
    return true;
}

bool RemoteProtocol::updateReferences(const std::vector<std::pair<std::string, std::string>>& ref_updates) {
    notifyReferenceUpdate("Updating " + std::to_string(ref_updates.size()) + " references");

    for (const auto& [ref_name, new_hash] : ref_updates) {
        if (updateReference(ref_name, new_hash)) {
            notifyReferenceUpdate("Updated reference " + ref_name + " -> " + new_hash);
        } else {
            notifyProtocolError("Failed to update reference: " + ref_name);
            return false;
        }
    }

    return true;
}

std::unordered_set<std::string> RemoteProtocol::findMissingObjects(const std::unordered_set<std::string>& client_has) {
    std::unordered_set<std::string> missing_objects;

    auto all_server_objects = getAllReferences();
    for (const auto& obj : all_server_objects) {
        if (!client_has.count(obj)) {
            missing_objects.insert(obj);
        }
    }

    return missing_objects;
}

bool RemoteProtocol::sendObjects(const std::unordered_set<std::string>& objects_to_send) {
    for (const auto& object_hash : objects_to_send) {
        std::string object_data = readObject(object_hash);
        if (!object_data.empty()) {
            sendLine("OBJECT " + object_hash);
            sendLine(object_data);
        }
    }
    sendLine("END_OBJECTS");
    return true;
}

bool RemoteProtocol::objectExists(const std::string& object_hash) const {
    fs::path objects_dir = repository_manager_->getRepositoryPath() / ".svcs" / "objects";
    fs::path object_path = objects_dir / object_hash.substr(0, 2) / object_hash.substr(2);
    return fs::exists(object_path);
}

std::string RemoteProtocol::readObject(const std::string& object_hash) const {
    fs::path objects_dir = repository_manager_->getRepositoryPath() / ".svcs" / "objects";
    fs::path object_path = objects_dir / object_hash.substr(0, 2) / object_hash.substr(2);

    if (!fs::exists(object_path)) {
        return "";
    }

    std::ifstream file(object_path, std::ios::binary);
    if (!file) {
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool RemoteProtocol::writeObject(const std::string& object_hash, const std::string& object_data) {
    fs::path objects_dir = repository_manager_->getRepositoryPath() / ".svcs" / "objects";
    fs::path object_dir = objects_dir / object_hash.substr(0, 2);
    fs::path object_path = object_dir / object_hash.substr(2);

    try {
        fs::create_directories(object_dir);
        std::ofstream file(object_path, std::ios::binary);
        if (!file) {
            return false;
        }
        file << object_data;
        return true;
    } catch (...) {
        return false;
    }
}

std::unordered_set<std::string> RemoteProtocol::getAllReferences() const {
    std::unordered_set<std::string> refs;

    fs::path refs_dir = repository_manager_->getRepositoryPath() / ".svcs" / "refs" / "heads";
    if (fs::exists(refs_dir)) {
        for (const auto& entry : fs::directory_iterator(refs_dir)) {
            if (entry.is_regular_file()) {
                std::ifstream file(entry.path());
                std::string hash;
                if (file >> hash) {
                    refs.insert(hash);
                }
            }
        }
    }

    return refs;
}

bool RemoteProtocol::updateReference(const std::string& ref_name, const std::string& new_hash) {
    fs::path ref_path = repository_manager_->getRepositoryPath() / ".svcs" / "refs" / "heads" / ref_name;

    try {
        std::ofstream file(ref_path);
        if (!file) {
            return false;
        }
        file << new_hash;
        return true;
    } catch (...) {
        return false;
    }
}

bool RemoteProtocol::readLine(std::string& line) const {
    if (!std::getline(std::cin, line)) {
        return false;
    }

    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }

    return true;
}

bool RemoteProtocol::sendLine(const std::string& line) const {
    notifyNetworkSend("Sending: " + line);
    std::cout << line << std::endl;
    return !std::cout.fail();
}

bool RemoteProtocol::sendData(const std::string& data) const {
    notifyNetworkSend("Sending data: " + std::to_string(data.size()) + " bytes");
    std::cout << data;
    std::cout.flush();
    return !std::cout.fail();
}

bool RemoteProtocol::sendError(const std::string& error) const {
    notifyProtocolError("Protocol error: " + error);
    std::cerr << "ERROR: " << error << std::endl;
    return !std::cerr.fail();
}

bool RemoteProtocol::isValidObjectHash(const std::string& hash) const {
    return hash.length() == 40 &&
           std::all_of(hash.begin(), hash.end(), [](char c) {
               return std::isxdigit(c);
           });
}

bool RemoteProtocol::isValidReference(const std::string& ref_name) const {
    return !ref_name.empty() &&
           ref_name.find("..") == std::string::npos &&
           ref_name.find('/') == std::string::npos &&
           ref_name.find('\\') == std::string::npos;
}

void RemoteProtocol::notifyProtocolStart(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::PROTOCOL_START, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyProtocolSuccess(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::PROTOCOL_SUCCESS, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyProtocolError(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::PROTOCOL_ERROR, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyNetworkSend(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::NETWORK_SEND, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyNetworkReceive(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::NETWORK_RECEIVE, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyObjectTransfer(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::OBJECT_TRANSFER, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyReferenceUpdate(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::REFERENCE_UPDATE, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyNegotiation(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::NEGOTIATION_PHASE, "[Protocol] " + message});
    }
}

bool RemoteProtocol::sendProtocolError(const std::string& error) const {
    notifyProtocolError(error);
    std::cerr << "ERROR: " << error << std::endl;
    return !std::cerr.fail();
}

void RemoteProtocol::notifyDebug(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::DEBUG_MESSAGE, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyInfo(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::GENERAL_INFO, "[Protocol] " + message});
    }
}

void RemoteProtocol::notifyError(const std::string& message) const {
    if (event_bus_) {
        event_bus_->notify({Event::Type::ERROR_MESSAGE, "[Protocol] " + message});
    }
}