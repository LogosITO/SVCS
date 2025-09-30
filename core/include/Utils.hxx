/**
 * @file Utils.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Defines utility functions for core of SVCS.
 */
#pragma once 

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

/**
 * @brief Reads the entire content of a file into a single std::string.
 * * This utility function is designed for reading small to medium-sized files
 * and is crucial for creating VcsObject contents (e.g., Blobs). It opens the 
 * file in binary mode to ensure all characters are read correctly across platforms.
 * @param full_path The full, absolute path to the file on disk.
 * @return The entire content of the file as a single std::string.
 * @throw std::runtime_error If the file cannot be opened for reading.
 */
std::string read_file_to_string(const fs::path& full_path);

/**
 * @brief Converts a binary buffer (e.g., a raw SHA-1 hash) into its hexadecimal string representation.
 * @param binary_data Pointer to the raw binary data.
 * @param length The length of the binary data in bytes (e.g., 20 for SHA-1).
 * @return A std::string containing the hexadecimal representation (e.g., 40 characters for SHA-1).
 */
std::string binary_to_hex_string(const unsigned char* binary_data, size_t length);

/**
 * @brief Converts a hexadecimal string (e.g., 40 chars) into its raw binary byte representation (e.g., 20 bytes).
 * @param hex_string The input hexadecimal string. Must have an even length.
 * @return A std::string containing the raw binary bytes.
 * @throw std::runtime_error if the input string has an odd length or contains invalid hex characters.
 */
std::string hex_to_binary_string(const std::string& hex_string);