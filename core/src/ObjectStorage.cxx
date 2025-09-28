#include "../include/ObjectStorage.hxx"

#include "../include/Blob.hxx"
#include "../include/Tree.hxx"
#include "../include/Commit.hxx"

#include <zconf.h>
#include <zlib.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

const size_t CHUNK_SIZE = 16384;

ObjectStorage::ObjectStorage(const std::string& root_path) :
    objects_dir(fs::path(root_path) / ".svcs" / "object") {
    if (!fs::exists(objects_dir)) {
        try {
            fs::create_directories(objects_dir);
        } catch (const fs::filesystem_error& e) {
            throw std::runtime_error("Failed to initialize object storage: " + std::string(e.what()));
        }
    }
}

std::string ObjectStorage::getObjectPath(const std::string& hash) const {
    if (hash.length() != 40) {
        throw std::runtime_error("Invalid hash length for object path!");
    }

    std::string dir_name = hash.substr(0, 2);
    std::string file_name = hash.substr(2);

    fs::path full_path = fs::path(objects_dir) / dir_name / file_name;
    return full_path.string();
}

std::string ObjectStorage::compress(const std::string& data) const {
    z_stream strm = {};
    auto defi = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    if (defi != Z_OK) {
        throw std::runtime_error("Zlib compression init failed.");
    }

    strm.next_in = (Bytef*)data.data();
    strm.avail_in = data.size();

    std::string compressed_data;
    char out_buffer[CHUNK_SIZE];
    int ret;

    do {
        strm.next_out = (Bytef*)out_buffer;
        strm.avail_out = CHUNK_SIZE;
        ret = deflate(&strm, Z_FINISH);

        compressed_data.append(out_buffer, CHUNK_SIZE - strm.avail_out);
    } while (ret == Z_OK);

    deflateEnd(&strm);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Zlib compression failed with error code:" + std::to_string(ret));
    }
    
    return compressed_data;
}

std::string ObjectStorage::decompress(const std::string& compressed_data) const {
    z_stream strm = {};

    if (inflateInit2(&strm, -MAX_WBITS) != Z_OK) {
        throw std::runtime_error("Zlib decompression init failed.");
    }

    strm.next_in = (Bytef*)compressed_data.data();
    strm.avail_in = compressed_data.size();

    std::string decompressed_data;

    decompressed_data.reserve(CHUNK_SIZE * 2); 
    char out_buffer[CHUNK_SIZE];
    int ret;
    
    do {
        strm.next_out = (Bytef*)out_buffer;
        strm.avail_out = CHUNK_SIZE;
        ret = inflate(&strm, Z_NO_FLUSH);

        if (ret < 0 && ret != Z_BUF_ERROR) {
            inflateEnd(&strm);
            throw std::runtime_error("Zlib decompression failed with error code: " + std::to_string(ret));
        }
        
        decompressed_data.append(out_buffer, CHUNK_SIZE - strm.avail_out);

    } while (ret == Z_OK);

    inflateEnd(&strm);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Zlib decompression failed: Corrupt or incomplete stream.");
    }

    return decompressed_data;
}

bool ObjectStorage::saveObject(const VcsObject& obj) const {
    std::string raw_data = obj.serialize();
    std::string header = obj.getType() + " " + std::to_string(raw_data.length()) + "\0";
    std::string final_content = header + raw_data;

    std::string hash = obj.getHashId(); 
    if (hash.empty() || hash.length() != 40) {
        throw std::runtime_error("Attempt to save object with invalid hash.");
    }

    std::string compressed_data = compress(final_content);
    
    std::string file_path = getObjectPath(hash);
    fs::path dir = fs::path(file_path).parent_path();
    
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }

    std::ofstream ofs(file_path, std::ios::binary | std::ios::trunc);
    if (!ofs) {
        throw std::runtime_error("Failed to open file for saving object: " + file_path);
    }
    ofs.write(compressed_data.data(), compressed_data.size());
    return true;
}

std::unique_ptr<VcsObject> ObjectStorage::loadObject(const std::string& hash) const {
    std::string file_path = getObjectPath(hash);

    std::ifstream ifs(file_path, std::ios::binary);
    if (!ifs) {
        throw std::runtime_error("Object not found at: " + file_path);
    }

    std::string compressed_data((std::istreambuf_iterator<char>(ifs)),
                                 std::istreambuf_iterator<char>());

    std::string final_content = decompress(compressed_data);
    
    std::string calculated_hash = VcsObject::calculateHash(final_content);
    if (calculated_hash != hash) {
        throw std::runtime_error("Object integrity error: Calculated hash mismatch! "
                             "Expected: " + hash + ", Got: " + calculated_hash);
    }   

    size_t null_byte_pos = final_content.find('\0');
    if (null_byte_pos == std::string::npos) {
        throw std::runtime_error("Object header missing null terminator.");
    }
    
    std::string header = final_content.substr(0, null_byte_pos);
    std::string content = final_content.substr(null_byte_pos + 1);

    std::stringstream header_ss(header);
    std::string type_str;
    size_t size_from_header;
    
    if (!(header_ss >> type_str >> size_from_header)) {
        throw std::runtime_error("Object header format error.");
    }

    if (size_from_header != content.length()) {
        throw std::runtime_error("Object integrity error: Content size mismatch.");
    }

    return createObjectFromContent(type_str, content);
}

std::unique_ptr<VcsObject> ObjectStorage::createObjectFromContent(
    const std::string& type, 
    const std::string& content
) const {
    if (type == "blob") {
        return std::make_unique<Blob>(content); 
    } else if (type == "tree") {
        return std::make_unique<Tree>(Tree::deserialize(content));
    } else if (type == "commit") {
        return std::make_unique<Commit>(Commit::deserialize(content));
    } else {
        throw std::runtime_error("Unknown VCS object type '" + type + "' in database.");
    }
}