#pragma once

#include <array>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace net {

inline std::string findcic(const std::string& url) {
    std::string result;

    std::string command =
        "curl -Ls -- \"" + url + "\"";

    FILE* pipe = popen(command.c_str(), "r");

    if (!pipe) {
        return "";
    }

    std::array<char, 4096> buffer{};

    while (fgets(buffer.data(), buffer.size(), pipe)) {
        result += buffer.data();
    }

    pclose(pipe);

    return result;
}

inline int open(const std::string& url) {
    std::string command =
        "xdg-open \"" + url + "\" >/dev/null 2>&1 &";

    return std::system(command.c_str());
}

}