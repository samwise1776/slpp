#pragma once

#include <efiles.hpp>
#include <string>
#include <iostream>
#include <fstream>

inline void log(
    std::string t, std::string l
) {
    std::string path = "logs/";
    std::filesystem::create_directories(path);

    efiles::append(path + "app.log", "[" + l + "]" + t);
}