#include "../include/Utils.hxx"

#include <fstream>
#include <sstream>

std::string read_file_to_string(const fs::path& full_path) {
    std::ifstream file(full_path, std::ios::in | std::ios::binary | std::ios::ate);
    
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for staging: " + full_path.string());
    }

    std::streamsize size = file.tellg();
    
    file.seekg(0, std::ios::beg);
    std::string buffer(size, '\0');
    if (file.read(&buffer[0], size)) {
        return buffer;
    } else {
        throw std::runtime_error("Failed to read all file content from: " + full_path.string());
    }
}

std::string binary_to_hex_string(const unsigned char* data, size_t length) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < length; ++i) {
        ss << std::setw(2) << static_cast<unsigned int>(data[i]);
    }
    return ss.str();
}

std::string hex_to_binary_string(const std::string& hex_string) {
    if (hex_string.length() % 2 != 0) {
        throw std::runtime_error("Hex string must have an even length for binary conversion.");
    }

    std::string binary_data;
    binary_data.reserve(hex_string.length() / 2);

    for (size_t i = 0; i < hex_string.length(); i += 2) {
        std::string byte_str = hex_string.substr(i, 2);
        
        unsigned int byte_value;
        std::stringstream ss;
        
        ss << std::hex << byte_str;
        
        if (!(ss >> byte_value)) {
            throw std::runtime_error("Invalid hexadecimal character in string.");
        }

        binary_data.push_back(static_cast<char>(byte_value));
    }

    return binary_data;
}