/**
 * @file Blob.cxx
 * @brief Implementation of the Blob class, a core SVCS object for file content storage.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/Blob.hxx"

Blob::Blob(const std::string& raw_data) : data(raw_data) {
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