#include "../include/Commit.hxx"
#include <sstream>
#include <algorithm>

Commit::Commit(
    std::string thash,
    std::vector<std::string> phashes,
    std::string auth,
    std::string msg,
    std::time_t tstamp
) :
    tree_hash(std::move(thash)),
    parent_hashes(std::move(phashes)),
    author(std::move(auth)),
    timestamp(tstamp),
    message(std::move(msg)) {
    std::sort(parent_hashes.begin(), parent_hashes.end());
    computeHash(serialize());
}

std::string Commit::getType() const {
    return "commit";
}

std::string Commit::serialize() const {
    std::stringstream ss;
    ss << "tree" << tree_hash << "\n";

    for(const auto& parent_hash : parent_hashes) {
        ss << "parent" << parent_hash << "\n";
    }

    ss << "author" << author << " " << timestamp << " +0000\n";
    ss << "committer" << author << " " << timestamp << " +0000\n";

    ss << "\n";
    ss << message << "\n";

    return ss.str();
}

Commit Commit::deserialize(const std::string& raw_content) {
    std::stringstream ss(raw_content);
    std::string line;
    
    std::string thash;
    std::vector<std::string> phashes;
    std::string auth;
    std::string committer_auth;
    std::time_t tstamp = 0;
    
    while (std::getline(ss, line) && !line.empty()) {
        
        size_t space_pos = line.find(' ');
        if (space_pos == std::string::npos) {
            continue; 
        }

        std::string key = line.substr(0, space_pos);
        std::string value = line.substr(space_pos + 1); 

        if (key == "tree") {
            thash = value;
        } else if (key == "parent") {
            phashes.push_back(value);
        } else if (key == "author" || key == "committer") {           
            size_t ts_end_pos = value.rfind(' '); 
            if (ts_end_pos == std::string::npos) continue;
            
            size_t ts_start_pos = value.rfind(' ', ts_end_pos - 1);
            if (ts_start_pos == std::string::npos) continue;
            
            std::string ts_str = value.substr(ts_start_pos + 1, ts_end_pos - ts_start_pos - 1);
            try {
                tstamp = (std::time_t)std::stoll(ts_str); 
            } catch (const std::exception& e) {
                throw std::runtime_error("Commit deserialization error: Failed to parse timestamp: " + std::string(e.what()));
            }
            
            std::string current_auth = value.substr(0, ts_start_pos);

            size_t last_char = current_auth.find_last_not_of(' ');
            if (last_char != std::string::npos) {
                current_auth = current_auth.substr(0, last_char + 1);
            }
            
            if (key == "author") {
                if (auth.empty()) {
                    auth = std::move(current_auth);
                }
            } else if (key == "committer") {
                committer_auth = std::move(current_auth);
            }
        }
    }

    std::string msg_full;
    std::string temp_message;
    while (std::getline(ss, line)) {
        temp_message += line + "\n";
    }

    if (!temp_message.empty() && temp_message.back() == '\n') {
        temp_message.pop_back();
    }
    msg_full = std::move(temp_message);

    if (thash.empty() || auth.empty()) {
        throw std::runtime_error("Commit deserialization error: Missing mandatory field (tree_hash or author).");
    }

    return Commit(
        std::move(thash),
        std::move(phashes),
        std::move(auth),
        std::move(msg_full),
        tstamp
    );
}

const std::string& Commit::getTreeHash() const {
    return tree_hash;
}

const std::vector<std::string>& Commit::getParentHashes() const {
    return parent_hashes;
}

const std::string& Commit::getAuthor() const {
    return author;
}

std::time_t Commit::getTimestamp() const {
    return timestamp;
}
const std::string& Commit::getMessage() const {
    return message;
}