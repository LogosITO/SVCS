#include "../include/Tree.hxx"
#include "../include/Utils.hxx"
#include <algorithm>
#include <sstream>

const size_t HASH_BYTE_SIZE = 32;

inline bool TreeEntry::operator<(const TreeEntry &other) const {
    return name < other.name;
}

TreeEntry createEntry(std::string name, std::string hash_id) {
    return TreeEntry{"100644", std::move(name), std::move(hash_id)};
}

Tree::Tree(std::vector<TreeEntry> entrs) : entries(std::move(entrs)) {
    std::sort(entries.begin(), entries.end());
    
    // 1. Получаем сырые данные
    std::string raw_content = this->serialize(); 

    // 2. Создаем ПОЛНОЕ содержимое объекта с заголовком
    std::string full_content = this->getType() + " " + 
                               std::to_string(raw_content.length()) + 
                               '\0' + 
                               raw_content;
    
    this->hash_id = calculateHash(full_content);
}


std::string Tree::serialize() const {
    std::stringstream ss(std::ios::binary | std::ios::out);
    
    for (const auto& entry : entries) { 
        ss << entry.mode << " " << entry.name;
        ss.put('\0'); 
        
        std::string binary_hash = hex_to_binary_string(entry.hash_id); 

        if (binary_hash.length() != HASH_BYTE_SIZE) {
            throw std::runtime_error("Tree serialization error: Invalid binary hash length. Expected 32 bytes, got " + std::to_string(binary_hash.length()));
        }
        ss.write(binary_hash.data(), HASH_BYTE_SIZE);
    }
    return ss.str();
}
Tree Tree::deserialize(const std::string &raw_content) {
    std::vector<TreeEntry> entries;
    std::istringstream ss(raw_content, std::ios::binary);
    
    char hash_buffer[HASH_BYTE_SIZE];
    
    while (ss.good()) {
        std::string mode_and_name_part;
        
        if (!std::getline(ss, mode_and_name_part, '\0')) {
            if (ss.eof() && mode_and_name_part.empty()) {
                break; 
            }
            throw std::runtime_error("Tree deserialization error: Missing name (NULL byte not found).");
        }

        size_t space_pos = mode_and_name_part.find(' ');
        if (space_pos == std::string::npos) {
            throw std::runtime_error("Tree deserialization error: Missing space delimiter in mode/name part.");
        }
        
        std::string mode_str = mode_and_name_part.substr(0, space_pos);
        std::string name_str = mode_and_name_part.substr(space_pos + 1);

        if (!ss.read(hash_buffer, HASH_BYTE_SIZE)) {
            if (!ss.eof()) {
                throw std::runtime_error("Tree deserialization error: Failed to read 32-byte hash.");
            }
            throw std::runtime_error("Tree deserialization error: Unexpected end of stream while reading hash.");
        }
        
        TreeEntry entry;
        entry.mode = mode_str;
        entry.name = name_str;
        
        entry.hash_id = binary_to_hex_string(
            reinterpret_cast<const unsigned char*>(hash_buffer),
            HASH_BYTE_SIZE
        );

        entries.push_back(std::move(entry));
    }
    
    return Tree(std::move(entries));
}

void Tree::addEntry(const TreeEntry& new_entry) {
    auto pred = [&new_entry](const TreeEntry& e) {
        return e.name == new_entry.name;
    };
    auto it = std::find_if(entries.begin(), entries.end(), pred);
    if (it != entries.end()) {
        *it = new_entry;
    } else {
        entries.push_back(new_entry);
    }
}

bool Tree::removeEntry(const std::string& name) {
    auto initial_size = entries.size();
    auto pred = [&name](const TreeEntry& entry) {
        return entry.name == name;
    };
    auto it = std::remove_if(entries.begin(), entries.end(), pred);
    entries.erase(it, entries.end());
    bool was_erased = entries.size() < initial_size;
    return was_erased;
}

std::string Tree::getType() const {
    return "tree";
}

std::optional<TreeEntry> Tree::findEntry(const std::string& name) const {
    auto pred = [&name](const TreeEntry& entry) {
        return entry.name == name;
    };
    auto it = find_if(entries.begin(), entries.end(), pred);
    if (it != entries.end()) {
        return *it;
    } else {
        return std::nullopt;
    }
}

const std::vector<TreeEntry>& Tree::getEntries() const {
    return entries;
}