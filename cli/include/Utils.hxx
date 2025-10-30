/**
 * @file Utils.hxx
 * @copyright
 * Copyright 2025 LogosITO
 * Licensed under MIT-License
 *
 * @english
 * @brief Utility functions for conditional debugging output.
 * @details This header defines the `printDebug` function, which provides a simple
 * mechanism for logging debug messages to standard output only when the `DEBUG_MODE`
 * preprocessor macro is defined.
 *
 * @russian
 * @brief Служебные функции для условного вывода отладки.
 * @details Этот заголовок определяет функцию `printDebug`, которая предоставляет простой
 * механизм для логирования отладочных сообщений в стандартный вывод только тогда, когда
 * определен макрос препроцессора `DEBUG_MODE`.
 */
#pragma once

#include <iostream>
#include <string>

/**
 * @english
 * @def DEBUG_MODE
 * @brief Preprocessor macro to enable or disable debug logging.
 * @details If this macro is defined (e.g., via a compiler flag like `-DDEBUG_MODE` or
 * by uncommenting the definition in this file), the printDebug() function will
 * output its messages. Otherwise, it compiles to a no-op.
 *
 * @russian
 * @def DEBUG_MODE
 * @brief Макрос препроцессора для включения или отключения отладочного логирования.
 * @details Если этот макрос определен (например, через флаг компилятора `-DDEBUG_MODE` или
 * раскомментировав определение в этом файле), функция printDebug() будет
 * выводить свои сообщения. В противном случае она компилируется в пустую операцию.
 */
//#define DEBUG_MODE // Uncommenting this line enables debug logging

/**
 * @english
 * @brief Conditionally prints a debug message to standard output.
 * @details The message is only printed if the DEBUG_MODE macro is defined
 * at the time of compilation. The function is marked as `inline` to suggest
 * inlining by the compiler, and is compiled to a no-op when DEBUG_MODE is
 * not defined, resulting in zero runtime overhead in release builds.
 * @param message The string message to output, prefixed with "DEBUG: ".
 *
 * @russian
 * @brief Условно выводит отладочное сообщение в стандартный вывод.
 * @details Сообщение выводится только если макрос DEBUG_MODE определен
 * во время компиляции. Функция помечена как `inline`, чтобы предложить
 * встраивание компилятором, и компилируется в пустую операцию, когда DEBUG_MODE
 * не определен, что приводит к нулевым накладным расходам во время выполнения в релизных сборках.
 * @param message Строковое сообщение для вывода, с префиксом "DEBUG: ".
 */
inline void printDebug(const std::string& message) {
    #ifdef DEBUG_MODE
        std::cout << "DEBUG: " << message << std::endl;
    #endif
}