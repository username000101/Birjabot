#pragma once

#include <iostream>

template <typename... ArgType> inline void log(ArgType&&... args) {
#ifndef NDEBUG
    std::cout << "[DEBUG] ";
    (std::cout << ... << args) << std::endl;
#endif
}

#define LOG log

#include <boost/locale.hpp>
#define convert(text) boost::locale::conv::to_utf<wchar_t>(text,"ru_RU.utf8")
