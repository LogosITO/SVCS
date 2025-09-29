#include "Logger.hxx"
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>

/**
 * @brief Вспомогательная функция для получения текущего времени.
 */
std::string getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    // Используем std::put_time для форматирования времени
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

/**
 * @brief Внутренний метод для форматирования и вывода сообщения.
 */
void Logger::log(const std::string& prefix, const std::string& message) {
    // Блокируем мьютекс, чтобы вывод не прерывался другим потоком
    std::lock_guard<std::mutex> lock(mtx_);
    
    std::cout << getCurrentTime() << " " << prefix << " " << message << std::endl;
}

/**
 * @brief Основной метод, реагирующий на события ядра.
 */
void Logger::notify(const Event& event) {
    switch (event.type) {
        case Event::FATAL_ERROR:
            // Вывод в stderr для критических ошибок
            std::cerr << getCurrentTime() << " [FATAL] " << event.details << " --- EXITING ---" << std::endl;
            break; 

        case Event::RUNTIME_ERROR:
            log("[ERROR]", "Operation failed: " + event.details);
            break;

        case Event::REPOSITORY_INIT_SUCCESS:
            log("[INFO]", "Repository successfully initialized at: " + event.details);
            break;

        case Event::OBJECT_WRITE_SUCCESS:
            log("[DEBUG]", "Object written: " + event.details);
            break;
            
        case Event::OBJECT_READ_SUCCESS:
            log("[DEBUG]", "Object read: " + event.details);
            break;

        case Event::INDEX_UPDATE:
            log("[INFO]", "Index updated. Files changed: " + event.details);
            break;
            
        case Event::COMMIT_CREATED:
            log("[INFO]", "New commit created: " + event.details);
            break;
            
        case Event::GENERAL_INFO:
            log("[INFO]", event.details);
            break;

        case Event::DEBUG_MESSAGE:
            log("[DEBUG]", event.details);
            break;

        default:
            log("[WARN]", "Received unknown event type.");
            break;
    }
}