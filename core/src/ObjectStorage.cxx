#include "../include/ObjectStorage.hxx"

#include "../include/Blob.hxx"
#include "../include/Tree.hxx"
#include "../include/Commit.hxx"
#include "../include/VcsObject.hxx" // Для calculateHash, если он там

#include <zconf.h>
#include <zlib.h>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>
#include <iterator>

namespace fs = std::filesystem;

const size_t CHUNK_SIZE = 16384;

// КРИТИЧЕСКИ ВАЖНО: VcsObject::calculateHash() ДОЛЖЕН быть реализован
// точно так же, как VcsObject::computeHash (SHA-256, 64 символа),
// включая использование заголовочной строки ('тип длина\0').

// =========================================================================
// PUBLIC METHODS
// =========================================================================

ObjectStorage::ObjectStorage(const std::string& root_path) :
    // ИСПРАВЛЕНИЕ: Папка должна быть 'objects', а не 'object'
    objects_dir(fs::path(root_path) / ".svcs" / "objects") {
    if (!fs::exists(objects_dir)) {
        try {
            fs::create_directories(objects_dir);
        } catch (const fs::filesystem_error& e) {
            throw std::runtime_error("Failed to initialize object storage: " + std::string(e.what()));
        }
    }
}

std::string ObjectStorage::getObjectPath(const std::string& hash) const {
    // ИСПРАВЛЕНИЕ: Изменяем ожидаемую длину хеша с 40 (SHA-1) на 64 (SHA-256)
    if (hash.length() != 64) {
        throw std::runtime_error("Invalid hash length for object path! Expected 64 characters (SHA-256).");
    }

    std::string dir_name = hash.substr(0, 2);
    std::string file_name = hash.substr(2);

    fs::path full_path = objects_dir / dir_name / file_name;
    return full_path.string();
}

bool ObjectStorage::saveObject(const VcsObject& obj) const {
    // 1. Создание полного содержимого (должно совпадать с тем, что использовалось для obj.getHashId())
    std::string raw_data = obj.serialize();
    // VcsObject::calculateHash должен был быть вызван с этим же заголовком!
    std::string header = obj.getType() + " " + std::to_string(raw_data.length()) + '\0';
    std::string final_content = header + raw_data;

    std::string hash = obj.getHashId(); 
    // ИСПРАВЛЕНИЕ: Изменяем ожидаемую длину хеша с 40 (SHA-1) на 64 (SHA-256)
    if (hash.empty() || hash.length() != 64) {
        throw std::runtime_error("Attempt to save object with invalid hash. Check VcsObject constructor and hash length (Expected 64).");
    }

    // 2. Компрессия
    std::string compressed_data = compress(final_content);
    
    // 3. Подготовка и запись файла
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

    // Чтение всего сжатого контента
    std::string compressed_data((std::istreambuf_iterator<char>(ifs)),
                                 std::istreambuf_iterator<char>());

    // 1. Декомпрессия (должна быть SHA-256)
    std::string final_content = decompress(compressed_data);
    
    // 2. КРИТИЧЕСКАЯ ПРОВЕРКА ЦЕЛОСТНОСТИ:
    // Хеш должен быть рассчитан на *полном* содержимом.
    std::string calculated_hash = VcsObject::calculateHash(final_content);
    
    if (calculated_hash != hash) {
        throw std::runtime_error("Object integrity error: Calculated hash mismatch! "
                             "Expected: " + hash + ", Got: " + calculated_hash + 
                             ". Content size: " + std::to_string(final_content.length()));
    }   

    // 3. Извлечение заголовка и содержимого
    size_t null_byte_pos = final_content.find('\0');
    if (null_byte_pos == std::string::npos) {
        throw std::runtime_error("Object header missing null terminator.");
    }
    
    std::string header = final_content.substr(0, null_byte_pos);
    std::string raw_content = final_content.substr(null_byte_pos + 1);

    // 4. Парсинг заголовка
    std::stringstream header_ss(header);
    std::string type_str;
    size_t size_from_header;
    
    if (!(header_ss >> type_str >> size_from_header)) {
        throw std::runtime_error("Object header format error: Failed to parse type and size.");
    }

    // 5. ПРОВЕРКА ДЛИНЫ: Убеждаемся, что декомпрессия не повредила данные
    if (size_from_header != raw_content.length()) {
        throw std::runtime_error("Object integrity error: Content size mismatch. Header size: " + 
                                std::to_string(size_from_header) + 
                                ", Actual size: " + std::to_string(raw_content.length()));
    }

    // 6. Создание объекта
    return createObjectFromContent(type_str, raw_content);
}



// =========================================================================
// PRIVATE/UTILITY METHODS
// =========================================================================

std::unique_ptr<VcsObject> ObjectStorage::createObjectFromContent(
    const std::string& type, 
    const std::string& content
) const {
    if (type == "blob") {
        return std::make_unique<Blob>(content); 
    } else if (type == "tree") {
        // ИСПРАВЛЕНИЕ: Гарантируем, что объект, возвращаемый deserialize, 
        // имеет корректный hash_id и корректно перемещается в unique_ptr.
        Tree temp_tree = Tree::deserialize(content);
        return std::make_unique<Tree>(std::move(temp_tree));
    } else if (type == "commit") {
        // ИСПРАВЛЕНИЕ: Применяем ту же логику для Commit.
        Commit temp_commit = Commit::deserialize(content);
        return std::make_unique<Commit>(std::move(temp_commit));
    } else {
        throw std::runtime_error("Unknown VCS object type '" + type + "' in database.");
    }
}

std::string ObjectStorage::compress(const std::string& data) const {
    z_stream strm = {};
    // Используем -MAX_WBITS для инициализации без заголовка zlib, как в Git
    auto defi = deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY);
    if (defi != Z_OK) {
        throw std::runtime_error("Zlib compression init failed.");
    }

    strm.next_in = (Bytef*)data.data();
    strm.avail_in = data.size();

    std::string compressed_data;
    compressed_data.reserve(data.size() * 2); // Резервируем место с запасом
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
        // Если ret == Z_BUF_ERROR, это обычно не фатально при правильном цикле
        throw std::runtime_error("Zlib compression failed with error code:" + std::to_string(ret));
    }
    
    return compressed_data;
}

std::string ObjectStorage::decompress(const std::string& compressed_data) const {
    z_stream strm = {};

    // Используем -MAX_WBITS для декомпрессии без заголовка zlib, как в Git
    if (inflateInit2(&strm, -MAX_WBITS) != Z_OK) {
        throw std::runtime_error("Zlib decompression init failed.");
    }

    strm.next_in = (Bytef*)compressed_data.data();
    strm.avail_in = compressed_data.size();

    std::string decompressed_data;

    // Резервируем место для оптимизации
    decompressed_data.reserve(compressed_data.size() * 3); 
    char out_buffer[CHUNK_SIZE];
    int ret;
    
    do {
        strm.next_out = (Bytef*)out_buffer;
        strm.avail_out = CHUNK_SIZE;
        ret = inflate(&strm, Z_NO_FLUSH);

        if (ret < 0 && ret != Z_BUF_ERROR && ret != Z_STREAM_END) {
            inflateEnd(&strm);
            throw std::runtime_error("Zlib decompression failed with error code: " + std::to_string(ret));
        }
        
        // Добавляем только реально заполненные байты
        decompressed_data.append(out_buffer, CHUNK_SIZE - strm.avail_out);

    } while (ret == Z_OK);

    inflateEnd(&strm);

    if (ret != Z_STREAM_END) {
        throw std::runtime_error("Zlib decompression failed: Corrupt or incomplete stream (ret=" + std::to_string(ret) + ").");
    }

    return decompressed_data;
}