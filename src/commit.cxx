#include "../include/commit.hxx"
#include <openssl/sha.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <utility>

Author::Author() :
    name("Anonym"), email("anonym@gmail.com") {}

Author::Author(std::string n, std::string e) {
    this->name = std::move(n);
    const std::regex pattern(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)");

    if (std::regex_match(e, pattern)) {
        this->email = std::move(e);
        std::cerr << "Warning: The provided email '" << e << "' is not valid! Using default email: anonym@gmail.com." << std::endl;
        this->email = "anonym@gmail.com"; 
    }
}

Author::Author(const Author& a) :
    name(a.name), email(a.email) {}

std::string Author::get_name() const { return this->name; }

std::string Author::get_email() const { return this->email; }

// --- Constructors ---

Commit::Commit() {
    this->author = Author();
    this->hash_id = "";
    this->user_message = "";
    this->timestamp = "";
    this->content_hash = "";
    this->file_path = "";
}

Commit::Commit(const Commit& c) {
    this->author = c.author;
    this->hash_id = c.hash_id;
    this->user_message = c.user_message;
    this->timestamp = c.timestamp;
    this->content_hash = c.content_hash;
    this->file_path = c.file_path;
}

Commit::Commit(
    Author auth, std::string hid, std::string umsg, 
    std::string tstamp, std::string chash, std::string fpath
) {
    // Используем std::move для оптимизации (там, где это безопасно)
    this->author = std::move(auth);
    this->hash_id = std::move(hid);
    this->user_message = std::move(umsg);
    this->timestamp = std::move(tstamp);
    this->content_hash = std::move(chash);
    this->file_path = std::move(fpath);
}

// --- Utility Functions ---

std::string Commit::calculate_hash(std::string content) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    SHA256((const unsigned char*) content.data(), content.length(), hash);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << (int) hash[i];
    }
    return ss.str();
}

// --- Getters ---

Author Commit::get_author() const {return this->author; }

std::string Commit::get_hash_id() const { return this->hash_id; }   

std::string Commit::get_user_message() const { return this->user_message; }

std::string Commit::get_timestamp() const { return this->timestamp; }

std::string Commit::get_content_hash() const { return this->content_hash; }

std::string Commit::get_file_path() const { return this->file_path; }