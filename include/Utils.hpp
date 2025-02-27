#pragma once

#include <iostream>

template <typename... ArgType> inline void log(ArgType&&... args) {
#ifndef NDEBUG
    std::cout << "[DEBUG] ";
    (std::cout << ... << args) << std::endl;
#endif
}

#define LOG log
