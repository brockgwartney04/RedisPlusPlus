#pragma once

#include <string>
#include <iostream>

void debug_print(std::string_view s) {
    for(char c : s) {
        switch(c) {
            case '\n': std::cout << "[LF]"; break;
            case '\r': std::cout << "[CR]"; break;
            case '\t': std::cout << "[TAB]"; break;
            default:   std::cout << c; break;
        }
    }
    std::cout << std::flush;
}