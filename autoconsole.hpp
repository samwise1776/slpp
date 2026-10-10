#pragma once

#include <iostream>
#include <string>
#include <functional>
#include <unordered_map>

namespace cppforge {

class AutoConsole {
private:
    std::string promptText = "$ ";
    std::string consoleTitle = "CPPFORGE Console";

    std::unordered_map<std::string, std::function<void()>> commands;

    bool running = false;

public:
    void title(const std::string& text) {
        consoleTitle = text;
    }

    void prompt(const std::string& text) {
        promptText = text;
    }

    void print(const std::string& text) {
        std::cout << text << '\n';
    }

    void command(const std::string& name, std::function<void()> action) {
        commands[name] = std::move(action);
    }

    void stop() {
        running = false;
    }

    void loop() {
        running = true;

        std::cout << consoleTitle << "\n";
        std::cout << "Type 'help' for commands.\n\n";

        while (running) {
            std::cout << promptText << std::flush;

            std::string input;

            if (!std::getline(std::cin, input)) {
                break;
            }

            if (input == "exit") {
                stop();
                std::cout << "Goodbye!\n";
            }
            else if (input == "help") {
                std::cout << "Available commands:\n";

                for (const auto& cmd : commands) {
                    std::cout << "  " << cmd.first << '\n';
                }

                std::cout << "  help\n";
                std::cout << "  exit\n";
            }
            else if (commands.count(input)) {
                commands.at(input)();
            }
            else if (!input.empty()) {
                std::cout << "Unknown command: " << input << '\n';
            }
        }

        running = false;
    }
};

}