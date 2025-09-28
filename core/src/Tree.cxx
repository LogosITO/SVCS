#include "../include/Tree.hxx"
#include <algorithm>
#include <sstream>

inline bool TreeEntry::operator<(const TreeEntry &other) const {
    return name < other.name;
}

TreeEntry createEntry(std::string name, std::string hash_id) {
    return TreeEntry{"100644", std::move(name), std::move(hash_id)};
}

Tree::Tree(std::vector<TreeEntry> entrs) : entries(std::move(entrs)) {
    std::sort(entries.begin(), entries.end());
    computeHash(serialize());
}

Tree Tree::deserialize(const std::string &raw_content) {
    std::vector<TreeEntry> entries;
    std::stringstream ss(raw_content);
    std::string line;

    while(std::getline(ss, line, '\n')) {
        if (line.empty()) continue;
        std::stringstream line_ss(line);
        TreeEntry entry;
        if (!(line_ss >> entry.mode >> entry.type >> entry.hash_id)) {
            throw std::runtime_error("Tree deserialization error: Invalid entry format.");
        }
        std::string name_part;
        if(!std::getline(line_ss, name_part)) {
            throw std::runtime_error("Tree deserialization error: Missing name for entry.");
        }
        size_t pos = name_part.find_first_not_of(' ');
        entry.name = name_part.substr(pos);
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

std::string Tree::serialize() const {
    std::stringstream ss;
    for (const auto& entry : entries) {
        ss << entry.mode << " " 
        << entry.type << " " 
        << entry.hash_id << " " 
        << entry.name << "\n";
    }
    return ss.str();
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