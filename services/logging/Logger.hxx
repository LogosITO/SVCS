/**
 * @file Logger.hxx
 * @copyright 
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 * @brief Defines the concrete Logger class, an implementation of IObserver.
 */
#pragma once

#include "../IObserver.hxx"
#include <mutex>

/**
 * @brief Concrete Observer responsible for message output.
 * * This class implements IObserver and formats events for output to the console 
 * or another stream.
 */
class Logger : public IObserver {
public:
    /**
     * @brief Implementation of the notify method from IObserver.
     * @param event Structure Event with information about what happened.
     */
    void notify(const Event& event) override;
    
private:
    /**
     * @brief Private function for formatting and outputting messages.
     * @param prefix Level prefix (e.g., "[INFO]", "[ERROR]").
     * @param message The message to output.
     */
    void log(const std::string& prefix, const std::string& message);
    
    // Mutex for safe output in a multi-threaded environment
    std::mutex mtx_; 
};

/**
 * @brief Вспомогательная функция для получения текущего времени.
 */
std::string getCurrentTime();
