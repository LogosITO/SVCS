/**
 * @file Logger.cxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Implementation of the Logger class for thread-safe logging system.
 * @details This file provides the concrete implementation for the @ref Logger class,
 * including Singleton pattern management, log level filtering, message formatting,
 * and event bus integration. The Logger automatically processes system events
 * from the Event Bus and provides direct logging methods for manual use.
 *
 * @russian
 * @brief Реализация класса Logger для потокобезопасной системы логирования.
 * @details Этот файл предоставляет конкретную реализацию для класса @ref Logger,
 * включая управление шаблоном Singleton, фильтрацию уровней логов, форматирование сообщений
 * и интеграцию с шиной событий. Logger автоматически обрабатывает системные события
 * из шины событий и предоставляет прямые методы логирования для ручного использования.
 */

#include "Logger.hxx"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <sstream>

namespace svcs::services {

// Статические члены
std::unordered_map<std::string, std::shared_ptr<Logger>> Logger::instances_;
std::mutex Logger::instances_mutex_;

Logger::Logger(const std::string& name)
    : name_(name), log_level_(LogLevel::INFO)
{
    setPattern("[%Y-%m-%d %H:%M:%S] [%n] [%l] %v");
}

Logger::~Logger() {
    std::lock_guard<std::mutex> lock(instances_mutex_);
    instances_.erase(name_);
}

// IObserver методы
void Logger::update(const Event& event) {
    switch (event.type) {
        case Event::FATAL_ERROR:
            fatal("[Event] " + event.details);
            break;
        case Event::RUNTIME_ERROR:
            error("[Event] " + event.details);
            break;
        case Event::REPOSITORY_INIT_SUCCESS:
            info("[Event] Repository initialized: " + event.details);
            break;
        case Event::OBJECT_WRITE_SUCCESS:
            debug("[Event] Object written: " + event.details);
            break;
        case Event::OBJECT_READ_SUCCESS:
            debug("[Event] Object read: " + event.details);
            break;
        case Event::INDEX_UPDATE:
            debug("[Event] Index updated: " + event.details);
            break;
        case Event::COMMIT_CREATED:
            info("[Event] Commit created: " + event.details);
            break;
        case Event::GENERAL_INFO:
            info("[Event] " + event.details);
            break;
        case Event::DEBUG_MESSAGE:
            debug("[Event] " + event.details);
            break;
        default:
            info("[Event] " + event.details);
            break;
    }
}

void Logger::notify(const Event& event) {
    // Оставляем пустым если не используется
}

// Singleton
std::shared_ptr<Logger> Logger::getInstance(const std::string& name) {
    std::lock_guard<std::mutex> lock(instances_mutex_);
    
    auto it = instances_.find(name);
    if (it != instances_.end()) {
        return it->second;
    }
    
    auto logger = std::make_shared<Logger>(name);
    instances_[name] = logger;
    return logger;
}

// Основные методы логирования
void Logger::debug(const std::string& message) {
    if (log_level_ <= LogLevel::DEBUG) {
        log(LogLevel::DEBUG, message);
    }
}

void Logger::info(const std::string& message) {
    if (log_level_ <= LogLevel::INFO) {
        log(LogLevel::INFO, message);
    }
}

void Logger::warn(const std::string& message) {
    if (log_level_ <= LogLevel::WARN) {
        log(LogLevel::WARN, message);
    }
}

void Logger::error(const std::string& message) {
    if (log_level_ <= LogLevel::ERROR) {
        log(LogLevel::ERROR, message);
    }
}

void Logger::fatal(const std::string& message) {
    log(LogLevel::FATAL, message);
}

// Внутренний метод логирования
void Logger::log(LogLevel level, const std::string& message) {
    std::string formatted_message = formatMessage(level, message);
    std::cout << formatted_message << std::endl;
}

// Форматирование сообщения
std::string Logger::formatMessage(LogLevel level, const std::string& message) {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    size_t pos = 0;
    
    while (pos < pattern_.length()) {
        if (pattern_[pos] == '%' && pos + 1 < pattern_.length()) {
            char specifier = pattern_[pos + 1];
            switch (specifier) {
                case 'Y': // Год
                    ss << std::put_time(std::localtime(&time_t), "%Y");
                    break;
                case 'm': // Месяц
                    ss << std::put_time(std::localtime(&time_t), "%m");
                    break;
                case 'd': // День
                    ss << std::put_time(std::localtime(&time_t), "%d");
                    break;
                case 'H': // Часы
                    ss << std::put_time(std::localtime(&time_t), "%H");
                    break;
                case 'M': // Минуты
                    ss << std::put_time(std::localtime(&time_t), "%M");
                    break;
                case 'S': // Секунды
                    ss << std::put_time(std::localtime(&time_t), "%S");
                    break;
                case 'n': // Имя логгера
                    ss << name_;
                    break;
                case 'l': // Уровень логирования
                    ss << levelToString(level);
                    break;
                case 'v': // Сообщение
                    ss << message;
                    break;
                default:
                    ss << '%' << specifier;
                    break;
            }
            pos += 2;
        } else {
            ss << pattern_[pos];
            pos += 1;
        }
    }
    
    return ss.str();
}

// Преобразование уровня в строку
std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

// Конфигурация
void Logger::setLevel(LogLevel level) {
    log_level_ = level;
}

LogLevel Logger::getLevel() const {
    return log_level_;
}

void Logger::setPattern(const std::string& pattern) {
    pattern_ = pattern;
}

// Очистка инстансов
void Logger::clearInstances() {
    std::lock_guard<std::mutex> lock(instances_mutex_);
    instances_.clear();
}

// Простые методы без форматирования
void Logger::flush() {
    std::cout.flush();
}

}