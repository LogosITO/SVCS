/***********************************************************************************************
 * @file Commit.сxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Implementation of the class Commit.
 ***********************************************************************************************/

#include "../include/commit.hxx"
#include <openssl/sha.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <regex>
#include <utility> // Для std::move

Author::Author() :
    name("Anonym"), email("anonym@gmail.com") {}

Author::Author(std::string n, std::string e) {
    this->name = std::move(n); // Используем std::move
    const std::regex pattern(R"((\w+)(\.|_)?(\w*)@(\w+)(\.(\w+))+)"); // Используем R-строку для паттерна
    
    // ВАЖНО: Проверяем входной параметр 'e', а не член класса 'email'.
    if (std::regex_match(e, pattern)) {
        this->email = std::move(e); // Используем std::move
    } else {
        std::cerr << "Warning: The provided email '" << e << "' is not valid! Using default email: anonym@gmail.com." << std::endl;
        // Если email невалиден, оставляем email как "anonym@gmail.com"
        this->email = "anonym@gmail.com"; 
    }
}

Author::Author(const Author& a) :
    name(a.name), email(a.email) {}

std::string Author::get_name() const { return this->name; }

std::string Author::get_email() const { return this->email; }

// --- Constructors ---

/**
 * @brief Copy constructor.
 * @param c The Commit object to copy.
 */
Commit::Commit(const Commit& c) {
    this->author = c.author;
    this->hash_id = c.hash_id;
    this->user_message = c.user_message;
    this->timestamp = c.timestamp;
    this->content_hash = c.content_hash;
    this->file_path = c.file_path;
}

/**
 * @brief Primary constructor for initializing a Commit object.
 * @param auth The Author who created the commit.
 * @param hid Unique commit ID (hash of metadata).
 * @param umsg User-provided commit message.
 * @param tstamp Commit creation timestamp.
 * @param chash SHA-256 hash of the file content.
 * @param fpath Path where the content (BLOB) is stored on disk.
 */
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

/**
 * @brief Calculates the cryptographic SHA-256 hash for the given content.
 *
 * This function uses the OpenSSL library to produce a unique and tamper-proof 
 * identifier for the file content, represented as a 64-character hexadecimal string.
 * @param content The string content to be hashed.
 * @return std::string The SHA-256 hash.
 */
std::string Commit::calculate_hash(std::string content) {
    // SHA256_DIGEST_LENGTH равно 32 байтам
    unsigned char hash[SHA256_DIGEST_LENGTH];
    
    // Вычисление хеша: data, length, output buffer
    SHA256((const unsigned char*) content.data(), content.length(), hash);
    
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    
    // Преобразование 32-байтового массива в 64-символьную шестнадцатеричную строку
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::setw(2) << (int) hash[i];
    }
    return ss.str();
}

// --- Getters ---

/**
 * @brief Returns the Author object associated with the commit.
 * @return Author The commit author.
 */
Author Commit::get_author() const {return this->author; }

/**
 * @brief Returns the unique commit identifier (hash).
 * @return std::string Commit ID hash.
 */
std::string Commit::get_hash_id() const { return this->hash_id; }   

/**
 * @brief Returns the message associated with the commit.
 * @return std::string Commit message.
 */
std::string Commit::get_user_message() const { return this->user_message; }

/**
 * @brief Returns the commit creation timestamp.
 * @return std::string Timestamp.
 */
std::string Commit::get_timestamp() const { return this->timestamp; }

/**
 * @brief Returns the file content hash (BLOB).
 * @return std::string Content hash.
 */
std::string Commit::get_content_hash() const { return this->content_hash; }

/**
 * @brief Returns the path to the stored file content in the repository.
 * @return std::string File path.
 */
std::string Commit::get_file_path() const { return this->file_path; }