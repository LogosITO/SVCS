/**
 * @file Commit.cxx
 * @brief Implementation of the Commit class, representing a history snapshot in the SVCS.
 *
 * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */
 
#include "../include/Commit.hxx"

#include <sstream>
#include <algorithm>
#include <chrono>

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

    std::string raw_content = this->Commit::serialize();

    std::string full_content = this->Commit::getType() + " " +
                               std::to_string(raw_content.length()) +
                               '\0' +
                               raw_content;

    this->hash_id = calculateHash(full_content);
}

std::string Commit::getType() const {
    return "commit";
}

std::string Commit::serialize() const {
    std::stringstream ss;
    ss << "tree " << tree_hash << "\n";

    for(const auto& parent_hash : parent_hashes) {
        ss << "parent " << parent_hash << "\n";
    }
    
    ss << "author " << author << " " << timestamp << " +0000\n";
    ss << "committer " << author << " " << timestamp << " +0000\n"; 

    ss << "\n";
    ss << message;
    
    return ss.str();
}

Commit Commit::deserialize(const std::string& raw_content) {
    std::istringstream ss(raw_content);
    std::string line;
    
    std::string thash;
    std::vector<std::string> phashes;
    std::string author_line;
    std::time_t tstamp = 0; // Будет извлечен из author_line/committer_line
    
    // --- 1. Чтение Заголовка ---
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
        } else if (key == "author") {
            author_line = std::move(value);
        } else if (key == "committer") {
            std::string committer_line;
            committer_line = std::move(value);
        }
    }

    std::string author_data; // "Имя <email>"
    
    // Функция для парсинга поля, чтобы избежать дублирования кода
    auto parse_user_field = [&](const std::string& field_value, std::string& out_name, std::time_t& out_time) {
        if (field_value.empty()) return;

        // Поиск последнего пробела (перед часовым поясом)
        size_t tz_start_pos = field_value.rfind(' '); 
        if (tz_start_pos == std::string::npos) return;

        // Поиск пробела перед меткой времени (timestamp)
        size_t ts_start_pos = field_value.rfind(' ', tz_start_pos - 1);
        if (ts_start_pos == std::string::npos) return;
        
        // 1. Извлекаем метку времени
        std::string ts_str = field_value.substr(ts_start_pos + 1, tz_start_pos - ts_start_pos - 1);
        try {
            out_time = static_cast<std::time_t>(std::stoll(ts_str));
        } catch (const std::exception& e) {
            throw std::runtime_error("Commit deserialization error: Failed to parse timestamp: " + std::string(e.what()));
        }
        
        // 2. Извлекаем имя и email
        out_name = field_value.substr(0, ts_start_pos);
        // Удаляем завершающий пробел между email и timestamp (если есть)
        if (!out_name.empty() && out_name.back() == ' ') {
            out_name.pop_back(); 
        }
    };

    // Парсим author_line. Здесь out_name - это author_data, out_time - это tstamp.
    parse_user_field(author_line, author_data, tstamp);

    // --- 3. Чтение Сообщения ---
    std::string message;
    if (ss.peek() != std::char_traits<char>::eof()) {
        // Читаем все оставшееся содержимое до конца файла (EOF)
        message.assign(
            (std::istreambuf_iterator<char>(ss)),
             std::istreambuf_iterator<char>()
        );
    }

    // Удаляем потенциально добавленный пустой строкой перевод строки в начале сообщения
    if (!message.empty() && message.front() == '\n') {
        message.erase(0, 1);
    }
    
    // ВАЖНО: Мы должны вернуть объект, используя только те данные,
    // которые фактически хранятся в Commit.
    
    if (thash.empty() || author_data.empty()) {
        throw std::runtime_error("Commit deserialization error: Missing mandatory field (tree_hash or author).");
    }

    return {
        std::move(thash),
        std::move(phashes),
        std::move(author_data), // Передаем только имя/email
        std::move(message),
        tstamp
    };
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

void Commit::setMessage(const std::string& msg) { 
        if (message.empty()) {
            throw std::invalid_argument("Commit message cannot be empty");
        }
        message = msg; 
    }
    
void Commit::setTimestamp(const std::time_t tstamp) { 
    timestamp = tstamp; 
}

void Commit::setTimestampNow() {
    timestamp = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
}

void Commit::setParent(const std::string& parent) { 
    parent_hashes.push_back(parent); 
}