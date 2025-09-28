#include "../include/object.hxx"
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