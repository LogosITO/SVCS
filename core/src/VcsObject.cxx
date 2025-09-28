#include "../include/VcsObject.hxx"
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>

void VcsObject::computeHash(const std::string& content) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    SHA256((const unsigned char*) content.data(), content.length(), hash);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << (int) hash[i];
    }
    this->hash_id = ss.str();
}

std::string VcsObject::getHashId() const {
    return this->hash_id;
}

std::string VcsObject::calculateHash(const std::string& content) {
    unsigned char digest[SHA_DIGEST_LENGTH];

    SHA1((const unsigned char*)content.c_str(), content.length(), digest);

    std::stringstream ss;
    ss << std::hex << std::noshowbase << std::setfill('0');
    
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        ss << std::setw(2) << (int)digest[i];
    }

    return ss.str();
}


TestableObject::TestableObject(const std::string& type, const std::string& data) :
    type_name(type), content_data(data) {
    computeHash(serialize());
}

std::string TestableObject::serialize() const {
    return content_data;
}

std::string TestableObject::getType() const {
    return type_name;
}