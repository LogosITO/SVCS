/**
 * @file Utils.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Defines utility functions for core of SVCS.
 *
 * @russian
 * @brief Определяет служебные функции для ядра SVCS.
 */
#pragma once

#include <filesystem>
#include <string>

namespace fs = std::filesystem;

/**
 * @english
 * @brief Reads the entire content of a file into a single std::string.
 * @details This utility function is designed for reading small to medium-sized files
 * and is crucial for creating VcsObject contents (e.g., Blobs). It opens the
 * file in binary mode to ensure all characters are read correctly across platforms.
 * @param full_path The full, absolute path to the file on disk.
 * @return The entire content of the file as a single std::string.
 * @throw std::runtime_error If the file cannot be opened for reading.
 *
 * @russian
 * @brief Читает всё содержимое файла в одну строку std::string.
 * @details Эта служебная функция предназначена для чтения файлов малого и среднего размера
 * и является критически важной для создания содержимого VcsObject (например, Blobs). Она открывает
 * файл в бинарном режиме, чтобы гарантировать корректное чтение всех символов на всех платформах.
 * @param full_path Полный, абсолютный путь к файлу на диске.
 * @return Всё содержимое файла в виде одной строки std::string.
 * @throw std::runtime_error Если файл не может быть открыт для чтения.
 */
std::string read_file_to_string(const fs::path& full_path);

/**
 * @english
 * @brief Converts a binary buffer (e.g., a raw SHA-1 hash) into its hexadecimal string representation.
 * @param binary_data Pointer to the raw binary data.
 * @param length The length of the binary data in bytes (e.g., 20 for SHA-1).
 * @return A std::string containing the hexadecimal representation (e.g., 40 characters for SHA-1).
 *
 * @russian
 * @brief Преобразует бинарный буфер (например, сырой хеш SHA-1) в его шестнадцатеричное строковое представление.
 * @param binary_data Указатель на сырые бинарные данные.
 * @param length Длина бинарных данных в байтах (например, 20 для SHA-1).
 * @return Строка std::string, содержащая шестнадцатеричное представление (например, 40 символов для SHA-1).
 */
std::string binary_to_hex_string(const unsigned char* binary_data, size_t length);

/**
 * @english
 * @brief Converts a hexadecimal string (e.g., 40 chars) into its raw binary byte representation (e.g., 20 bytes).
 * @param hex_string The input hexadecimal string. Must have an even length.
 * @return A std::string containing the raw binary bytes.
 * @throw std::runtime_error if the input string has an odd length or contains invalid hex characters.
 *
 * @russian
 * @brief Преобразует шестнадцатеричную строку (например, 40 символов) в её сырое бинарное байтовое представление (например, 20 байт).
 * @param hex_string Входная шестнадцатеричная строка. Должна иметь чётную длину.
 * @return Строка std::string, содержащая сырые бинарные байты.
 * @throw std::runtime_error если входная строка имеет нечётную длину или содержит недопустимые шестнадцатеричные символы.
 */
std::string hex_to_binary_string(const std::string& hex_string);

/**
 * @english
 * @brief Computes the SHA-256 hash of the given input data.
 * @param input The input data to hash.
 * @return std::string The SHA-256 hash as a hexadecimal string.
 *
 * @russian
 * @brief Вычисляет хеш SHA-256 для заданных входных данных.
 * @param input Входные данные для хеширования.
 * @return std::string Хеш SHA-256 в виде шестнадцатеричной строки.
 */
std::string compute_sha256(const std::string& input);