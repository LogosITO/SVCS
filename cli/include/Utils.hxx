/**
 * @file DebugUtils.hxx
 * @brief Utility functions for conditional debugging output.
 *
 * @details This header defines the `printDebug` function, which provides a simple 
 * mechanism for logging debug messages to standard output only when the `DEBUG_MODE` 
 * preprocessor macro is defined.
 * * @copyright **Copyright (c) 2025 LogosITO**
 * @license **MIT License**
 */

#pragma once

#include <iostream>
#include <string>

/**
 * @def DEBUG_MODE
 * @brief Preprocessor macro to enable or disable debug logging.
 *
 * If this macro is defined (e.g., via a compiler flag like `-DDEBUG_MODE` or 
 * by uncommenting the definition in this file), the printDebug() function will 
 * output its messages. Otherwise, it compiles to a no-op.
 */
#define DEBUG_MODE // Uncommenting this line enables debug logging

/**
 * @brief Conditionally prints a debug message to standard output.
 *
 * @details The message is only printed if the DEBUG_MODE macro is defined 
 * at the time of compilation. The function is marked as `inline` to suggest 
 * inlining by the compiler, and is compiled to a no-op when DEBUG_MODE is 
 * not defined, resulting in zero runtime overhead in release builds.
 *
 * @param message The string message to output, prefixed with "DEBUG: ".
 */
inline void printDebug(std::string message) {
    #ifdef DEBUG_MODE
        std::cout << "DEBUG: " << message << std::endl;
    #endif
}