#pragma once

#include <iostream>
#include <string>

#define DEBUG_MODE

inline void printDebug(std::string message) {
    #ifdef DEBUG_MODE
        std::cout << "DEBUG: " << message << std::endl;
    #endif
}