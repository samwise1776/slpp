#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <filesystem>
#include <unordered_map>
#include <stdexcept>
#include <efiles.hpp>

namespace slpp {

    inline std::unordered_map<std::string, std::string> variables;

    inline void print(const std::string& text) {
        std::cout << text << "\n";
    }

    inline void printnnl(const std::string& text) {
        std::cout << text;
    }

    // Execute a command and return its output
    inline std::string execute(const std::string& cmd) {

        if (cmd == "lf -fo") {
            std::ostringstream output;

            // Capture output from efiles
            auto* old = std::cout.rdbuf(output.rdbuf());

            try {
                efiles::list_dirs(".");
            }
            catch (...) {
                std::cout.rdbuf(old);
                throw;
            }

            std::cout.rdbuf(old);

            return output.str();
        }

        else if (cmd == "pwd") {
            return std::filesystem::current_path().string() + "\n";
        }

        throw std::runtime_error("ECMD: Unknown command: " + cmd);
    }

    inline void ecmd(const std::string& cmd) {

        // Syntax: %s NAME = $(COMMAND)
        if (cmd.rfind("%s ", 0) == 0) {

            size_t equal = cmd.find(" = $(");

            if (equal == std::string::npos ||
                cmd.empty() ||
                cmd.back() != ')') {
                std::cerr << "ECMD: Invalid variable syntax\n";
                return;
            }

            std::string name = cmd.substr(3, equal - 3);

            if (name.empty()) {
                std::cerr << "ECMD: Missing variable name\n";
                return;
            }

            size_t start = equal + 5;

            std::string command = cmd.substr(
                start,
                cmd.size() - start - 1
            );

            try {
                variables[name] = execute(command);
            }
            catch (const std::exception& e) {
                std::cerr << e.what() << "\n";
            }

            return;
        }

        try {
            std::cout << execute(cmd);
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << "\n";
        }
    }

    inline std::string get(const std::string& name) {
        auto it = variables.find(name);

        if (it != variables.end()) {
            return it->second;
        }

        return "";
    }
}