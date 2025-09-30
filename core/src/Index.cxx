#include "../include/Index.hxx"
#include "../include/Blob.hxx"
#include "../include/Utils.hxx"
#include "../include/Tree.hxx"
#include "../include/ObjectStorage.hxx" 
#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <map>

namespace fs = std::filesystem;

void Index::getFileMetaData(const fs::path& full_path, long long& size, fs::file_time_type& mtime) {
    try {
        if (fs::exists(full_path) && !fs::is_directory(full_path)) {
            size = fs::file_size(full_path);
            mtime = fs::last_write_time(full_path);
        } else {
            size = 0;
            mtime = fs::file_time_type::min(); 
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error getting metadata for " << full_path.string() << ": " << e.what() << std::endl;
        size = 0;
        mtime = fs::file_time_type::min();
    }
}

Index::Index(const fs::path& vcs_root_path, const fs::path& repo_root_path, ObjectStorage& storage) 
    : index_file_path(vcs_root_path / "index"),
      repo_root_path(repo_root_path),
      storage_(storage)
{
    load();
}

Index::~Index() {}

void Index::addEntry(const IndexEntry& entry) {
    entries[entry.file_path] = entry;
}

const IndexEntry* Index::getEntry(const fs::path& file_path) const {
    auto it = entries.find(file_path);
    if (it == entries.end()) {
        return nullptr;
    }
    return &it->second;
}

void Index::stage_file(const fs::path& relative_path) {
    fs::path full_path = repo_root_path / relative_path;

    if (!fs::exists(full_path) || fs::is_directory(full_path)) {
        throw std::runtime_error("Cannot stage: path is invalid or is a directory: " + full_path.string());
    }

    std::string file_content;
    try {
        file_content = read_file_to_string(full_path);
    } catch (const std::exception& e) {
        throw std::runtime_error("Error reading file content: " + std::string(e.what()));
    }

    Blob blob(file_content);
    std::string blob_hash = blob.getHashId();

    storage_.saveObject(blob); 

    IndexEntry new_entry;
    long long current_size;
    fs::file_time_type current_mtime;
    getFileMetaData(full_path, current_size, current_mtime);

    new_entry.file_path = relative_path;
    new_entry.blob_hash = blob_hash;
    new_entry.file_size = current_size;
    new_entry.last_modified = current_mtime; 

    addEntry(new_entry);
    save();
}

bool Index::isFileModified(const fs::path& file_path) const {
    const IndexEntry* entry = getEntry(file_path);
    fs::path full_path = repo_root_path / file_path; 

    // Файл удален
    if (entry && !fs::exists(full_path)) {
        return true; 
    }

    // Новый файл
    if (!entry && fs::exists(full_path) && !fs::is_directory(full_path)) {
        return true; 
    }
    
    // Файл существует и есть в индексе
    if (entry && fs::exists(full_path) && !fs::is_directory(full_path)) {
        long long current_size;
        fs::file_time_type current_mtime;
        getFileMetaData(full_path, current_size, current_mtime);

        // Проверяем размер
        if (current_size != entry->file_size) {
            return true;
        }

        // Проверяем время модификации
        using std::chrono::time_point_cast;
        using std::chrono::seconds;

        bool time_differs = current_mtime != entry->last_modified; 

        if (time_differs) {
            // Если время изменилось, проверяем хеш
            try {
                std::string current_content = read_file_to_string(full_path);
                Blob current_blob(current_content);
                std::string current_hash = current_blob.getHashId();
                
                if (current_hash != entry->blob_hash) {
                    return true;
                }
            } catch (const std::exception& e) {
                return true; 
            }
        }
        
        return false;
    }
    
    return false;
}

void Index::save() const {
    std::ofstream ofs(index_file_path, std::ios::out | std::ios::trunc); 
    if (!ofs.is_open()) {
        throw std::runtime_error("Error: Could not open index file for writing.");
    }

    for (const auto& pair : entries) {
        const IndexEntry& entry = pair.second;
        
        using std::chrono::time_point_cast;
        using std::chrono::seconds;
        
        auto duration = time_point_cast<seconds>(entry.last_modified).time_since_epoch();
        long long mtime_ticks = duration.count();

        ofs << entry.blob_hash << " " 
            << entry.file_size << " " 
            << mtime_ticks << " " 
            << entry.file_path.generic_string() << "\n";
    }
}

void Index::load() {
    if (!fs::exists(index_file_path)) {
        return;
    }

    std::ifstream ifs(index_file_path, std::ios::in);
    if (!ifs.is_open()) {
        throw std::runtime_error("Error: Could not open index file for reading.");
    }

    entries.clear();

    std::string line;
    while (std::getline(ifs, line)) {
        std::stringstream ss(line);
        IndexEntry entry;
        
        if (!(ss >> entry.blob_hash >> entry.file_size)) {
            continue;
        }

        long long mtime_ticks;
        if (!(ss >> mtime_ticks)) {
            continue;
        }

        std::string path_part;
        std::getline(ss, path_part);
        
        if (!path_part.empty() && path_part[0] == ' ') {
            path_part = path_part.substr(1);
        }
        
        if (path_part.empty()) {
            continue;
        }

        entry.file_path = path_part;
        
        using std::chrono::seconds;
        entry.last_modified = fs::file_time_type(seconds(mtime_ticks));
        
        entries[entry.file_path] = entry;
    }
}

bool Index::write() const {
    std::ofstream ofs(index_file_path, std::ios::out | std::ios::trunc); 
    if (!ofs.is_open()) {
        throw std::runtime_error("std::runtime_error If the index file cannot be written.");
        return false;
    }

    for (const auto& pair : entries) {
        const IndexEntry& entry = pair.second;
        
        using std::chrono::time_point_cast;
        using std::chrono::seconds;

        auto duration = time_point_cast<seconds>(entry.last_modified).time_since_epoch();
        long long mtime_ticks = duration.count();

        ofs << entry.blob_hash << " " 
            << entry.file_size << " " 
            << mtime_ticks << " " 
            << entry.file_path.generic_string() << "\n";
    }

    if (ofs.fail()) {
        return false;
    }
    
    return true;
}

std::string Index::createTreeObject() {
    if (entries.empty()) {
        throw std::runtime_error("Cannot create a commit: the index is empty.");
    }
    
    std::map<fs::path, std::vector<IndexEntry>> staged_by_directory;
    
    for (const auto& pair : entries) {
        fs::path dir = pair.first.parent_path();
        staged_by_directory[dir].push_back(pair.second);
    }

    std::map<fs::path, std::string> tree_hashes;

    std::vector<fs::path> paths_to_process;
    for (const auto& pair : staged_by_directory) {
        paths_to_process.push_back(pair.first);
    }
    
    // Сортируем по длине пути (от самых длинных к самым коротким)
    std::sort(paths_to_process.begin(), paths_to_process.end(), [](const fs::path& a, const fs::path& b) {
        return a.generic_string().length() > b.generic_string().length();
    });

    for (const fs::path& current_dir : paths_to_process) {
        std::vector<TreeEntry> current_tree_entries;

        // Добавляем файлы из текущей директории
        for (const IndexEntry& entry : staged_by_directory[current_dir]) {
            if (entry.file_path.parent_path() == current_dir) {
                TreeEntry te;
                te.mode = "100644";
                te.name = entry.file_path.filename().string();
                te.hash_id = entry.blob_hash;
                current_tree_entries.push_back(te);
            }
        }

        // Добавляем поддиректории
        for (auto it = tree_hashes.begin(); it != tree_hashes.end(); ) {
            const fs::path& sub_tree_path = it->first;
            const std::string& sub_tree_hash = it->second;

            if (sub_tree_path.parent_path() == current_dir) {
                TreeEntry te;
                te.mode = "040000";
                te.name = sub_tree_path.filename().string();
                te.hash_id = sub_tree_hash;
                current_tree_entries.push_back(te);
                
                it = tree_hashes.erase(it);
            } else {
                ++it;
            }
        }
        
        // Сортируем записи
        std::sort(current_tree_entries.begin(), current_tree_entries.end());

        Tree current_tree(current_tree_entries); 
        std::string tree_hash = current_tree.getHashId(); 
        storage_.saveObject(current_tree); 
        tree_hashes[current_dir] = tree_hash;
    }

    // Ищем корневую директорию
    auto root_it = tree_hashes.find(fs::path{});
    if (root_it == tree_hashes.end()) {
        throw std::runtime_error("Root tree object was not created successfully.");
    }
    
    return root_it->second;
}