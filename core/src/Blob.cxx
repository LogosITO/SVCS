/**
* @file Blob.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the Blob class, a core SVCS object for file content storage.
 *
 * @russian
 * @brief Реализация класса Blob, основного объекта SVCS для хранения содержимого файлов.
 */

#include <utility>

#include "../include/Blob.hxx"


namespace svcs::core {

Blob::Blob(std::string  raw_data) : data(std::move(raw_data)) {
    std::string raw_content = this->serialize(); 
    std::string full_content = this->getType() + " " + 
                               std::to_string(raw_content.length()) + 
                               '\0' + 
                               raw_content;
    this->hash_id = calculateHash(full_content);
}

std::string Blob::getType() const {
    return "blob";
}

std::string Blob::serialize() const {
    return data;
}

const std::string& Blob::getData() const {
    return data;
}

}