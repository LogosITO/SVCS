/**
 * @file Utils.cxx
 * @brief Implementation of general utility functions for file I/O and hash format conversion.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#include "../include/VcsObject.hxx"

#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <utility>

void VcsObject::computeHash(const std::string& content) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    SHA256(reinterpret_cast<const unsigned char *>(content.data()), content.length(), hash);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for(unsigned char i : hash) {
        ss << std::setw(2) << static_cast<int>(i);
    }
    this->hash_id = ss.str();
}

std::string VcsObject::getHashId() const {
    return this->hash_id;
}

std::string VcsObject::calculateHash(const std::string& content) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    SHA256(reinterpret_cast<const unsigned char *>(content.data()), content.length(), hash);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for(unsigned char i : hash) {
        ss << std::setw(2) << static_cast<int>(i);
    }
    return ss.str();
}


TestableObject::TestableObject(std::string  type, std::string  data) :
    type_name(std::move(type)), content_data(std::move(data)) {
    computeHash(TestableObject::serialize());
}

std::string TestableObject::serialize() const {
    return content_data;
}

std::string TestableObject::getType() const {
    return type_name;
}