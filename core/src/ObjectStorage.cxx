#include "../include/ObjectStorage.hxx"
#include "../../services/ISubject.hxx"
#include "../../services/Event.hxx"
#include "../include/Blob.hxx"
#include "../include/Tree.hxx"
#include "../include/Commit.hxx"
#include "../include/VcsObject.hxx"

#include <zconf.h>
#include <zlib.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace fs = std::filesystem;

const size_t CHUNK_SIZE = 16384;

ObjectStorage::ObjectStorage(const std::string& root_path, ISubject* subject) :
    objects_dir(fs::path(root_path) / ".svcs" / "objects"),
    subject(subject)
{
    if (!fs::exists(objects_dir)) {
        try {
            fs::create_directories(objects_dir);
            if (subject) {
                Event e;
                e.type = Event::GENERAL_INFO;
                e.details = "Object storage directory created: " + objects_dir.string();
                subject->notify(e);
            }
        } catch (const fs::filesystem_error& e) {
            if (subject) {
                Event error_e;
                error_e.type = Event::FATAL_ERROR;
                error_e.details = "Failed to initialize object storage: " + std::string(e.what());
                subject->notify(error_e);
            }
            throw std::runtime_error("Failed to initialize object storage: " + std::string(e.what()));
        }
    }
}

std::string ObjectStorage::getObjectPath(const std::string& hash) const {
    if (hash.length() != 64) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Invalid hash length for object path! Expected 64 characters (SHA-256). Hash: " + hash;
            subject->notify(error_e);
        }
        throw std::runtime_error("Invalid hash length for object path! Expected 64 characters (SHA-256).");
    }

    std::string dir_name = hash.substr(0, 2);
    std::string file_name = hash.substr(2);

    fs::path full_path = objects_dir / dir_name / file_name;
    return full_path.string();
}

bool ObjectStorage::saveObject(const VcsObject& obj) const {
    std::string raw_data = obj.serialize();
    std::string header = obj.getType() + " " + std::to_string(raw_data.length()) + '\0';
    std::string final_content = header + raw_data;

    std::string hash = obj.getHashId(); 
    if (hash.empty() || hash.length() != 64) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Attempt to save object with invalid hash. Hash: " + (hash.empty() ? "(empty)" : hash.substr(0, 8) + "...");
            subject->notify(error_e);
        }
        throw std::runtime_error("Attempt to save object with invalid hash. Check VcsObject constructor and hash length (Expected 64).");
    }

    std::string compressed_data;
    try {
        compressed_data = compress(final_content);
    } catch (const std::exception& e) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Compression failed for object " + hash.substr(0, 8) + ": " + e.what();
            subject->notify(error_e);
        }
        throw;
    }
    
    std::string file_path = getObjectPath(hash);
    fs::path dir = fs::path(file_path).parent_path();
    
    if (!fs::exists(dir)) {
        try {
            fs::create_directories(dir);
        } catch (const fs::filesystem_error& e) {
            if (subject) {
                Event error_e;
                error_e.type = Event::RUNTIME_ERROR;
                error_e.details = "Failed to create directory for object " + hash.substr(0, 8) + ": " + std::string(e.what());
                subject->notify(error_e);
            }
            throw std::runtime_error("Failed to create directory for object: " + file_path + ": " + std::string(e.what()));
        }
    }

    std::ofstream ofs(file_path, std::ios::binary | std::ios::trunc);
    if (!ofs) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Failed to open file for saving object: " + file_path;
            subject->notify(error_e);
        }
        throw std::runtime_error("Failed to open file for saving object: " + file_path);
    }
    ofs.write(compressed_data.data(), compressed_data.size());

    if (subject) {
        Event e;
        e.type = Event::OBJECT_WRITE_SUCCESS;
        e.details = "Object saved: " + hash.substr(0, 8) + " type=" + obj.getType() + " size=" + std::to_string(raw_data.length());
        subject->notify(e);
    }

    return true;
}

std::unique_ptr<VcsObject> ObjectStorage::loadObject(const std::string& hash) const {
    std::string file_path = getObjectPath(hash);

    std::ifstream ifs(file_path, std::ios::binary);
    if (!ifs) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Object not found at: " + file_path;
            subject->notify(error_e);
        }
        throw std::runtime_error("Object not found at: " + file_path);
    }

    std::string compressed_data((std::istreambuf_iterator<char>(ifs)),
                                 std::istreambuf_iterator<char>());

    std::string final_content;
    try {
        final_content = decompress(compressed_data);
    } catch (const std::exception& e) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Decompression failed for object " + hash.substr(0, 8) + ": " + e.what();
            subject->notify(error_e);
        }
        throw;
    }
    
    std::string calculated_hash = VcsObject::calculateHash(final_content);
    
    if (calculated_hash != hash) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Object integrity error: Hash mismatch for " + hash.substr(0, 8) + ". Calculated: " + calculated_hash.substr(0, 8);
            subject->notify(error_e);
        }
        throw std::runtime_error("Object integrity error: Calculated hash mismatch!");
    }   

    size_t null_byte_pos = final_content.find('\0');
    if (null_byte_pos == std::string::npos) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Object header missing null terminator for " + hash.substr(0, 8);
            subject->notify(error_e);
        }
        throw std::runtime_error("Object header missing null terminator.");
    }
    
    std::string header = final_content.substr(0, null_byte_pos);
    std::string raw_content = final_content.substr(null_byte_pos + 1);

    std::stringstream header_ss(header);
    std::string type_str;
    size_t size_from_header;
    
    if (!(header_ss >> type_str >> size_from_header)) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Object header format error for " + hash.substr(0, 8) + ". Header: " + header;
            subject->notify(error_e);
        }
        throw std::runtime_error("Object header format error: Failed to parse type and size.");
    }

    if (size_from_header != raw_content.length()) {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Content size mismatch for " + hash.substr(0, 8) + ". Header size: " + std::to_string(size_from_header) + ", Actual: " + std::to_string(raw_content.length());
            subject->notify(error_e);
        }
        throw std::runtime_error("Object integrity error: Content size mismatch.");
    }

    std::unique_ptr<VcsObject> obj = createObjectFromContent(type_str, raw_content);

    if (subject) {
        Event e;
        e.type = Event::OBJECT_READ_SUCCESS;
        e.details = "Object loaded: " + hash.substr(0, 8) + " type=" + type_str;
        subject->notify(e);
    }

    return obj;
}

std::unique_ptr<VcsObject> ObjectStorage::createObjectFromContent(
    const std::string& type, 
    const std::string& content
) const {
    if (type == "blob") {
        return std::make_unique<Blob>(content); 
    } else if (type == "tree") {
        Tree temp_tree = Tree::deserialize(content);
        return std::make_unique<Tree>(std::move(temp_tree));
    } else if (type == "commit") {
        Commit temp_commit = Commit::deserialize(content);
        return std::make_unique<Commit>(std::move(temp_commit));
    } else {
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = "Unknown VCS object type '" + type + "' in database.";
            subject->notify(error_e);
        }
        throw std::runtime_error("Unknown VCS object type '" + type + "' in database.");
    }
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
    compressed_data.reserve(data.size() * 2);
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
        std::string error_msg = "Zlib compression failed with error code:" + std::to_string(ret);
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = error_msg;
            subject->notify(error_e);
        }
        throw std::runtime_error(error_msg);
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

    decompressed_data.reserve(compressed_data.size() * 3); 
    char out_buffer[CHUNK_SIZE];
    int ret;
    
    do {
        strm.next_out = (Bytef*)out_buffer;
        strm.avail_out = CHUNK_SIZE;
        ret = inflate(&strm, Z_NO_FLUSH);

        if (ret < 0 && ret != Z_BUF_ERROR && ret != Z_STREAM_END) {
            inflateEnd(&strm);
            std::string error_msg = "Zlib decompression failed with error code: " + std::to_string(ret);
            if (subject) {
                Event error_e;
                error_e.type = Event::RUNTIME_ERROR;
                error_e.details = error_msg;
                subject->notify(error_e);
            }
            throw std::runtime_error(error_msg);
        }
        
        decompressed_data.append(out_buffer, CHUNK_SIZE - strm.avail_out);

    } while (ret == Z_OK);

    inflateEnd(&strm);

    if (ret != Z_STREAM_END) {
        std::string error_msg = "Zlib decompression failed: Corrupt or incomplete stream (ret=" + std::to_string(ret) + ").";
        if (subject) {
            Event error_e;
            error_e.type = Event::RUNTIME_ERROR;
            error_e.details = error_msg;
            subject->notify(error_e);
        }
        throw std::runtime_error(error_msg);
    }

    return decompressed_data;
}

bool ObjectStorage::objectExists(const std::string& hash) const {
    try {
        std::string file_path = getObjectPath(hash);
        return fs::exists(file_path);
    } catch (const std::exception& e) {
        return false;
    }
}
