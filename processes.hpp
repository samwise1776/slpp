#pragma once

#include <algorithm>
#include <csignal>
#include <dirent.h>
#include <fstream>
#include <string>
#include <vector>

namespace processes {

struct Process {
    int pid;
    std::string name;
};

inline bool isNumber(const std::string& s) {
    return !s.empty() &&
           std::all_of(s.begin(), s.end(), ::isdigit);
}

inline std::string getName(int pid) {
    std::ifstream file(
        "/proc/" + std::to_string(pid) + "/comm"
    );

    std::string name;

    if (file) {
        std::getline(file, name);
    }

    return name;
}

inline std::vector<Process> list() {
    std::vector<Process> result;

    DIR* dir = opendir("/proc");

    if (!dir) {
        return result;
    }

    dirent* entry;

    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;

        if (!isNumber(name)) {
            continue;
        }

        int pid = std::stoi(name);

        result.push_back({
            pid,
            getName(pid)
        });
    }

    closedir(dir);

    return result;
}

inline bool close(int pid) {
    return kill(pid, SIGTERM) == 0;
}

inline bool forceClose(int pid) {
    return kill(pid, SIGKILL) == 0;
}

inline std::vector<int> find(const std::string& processName) {
    std::vector<int> pids;

    for (const auto& process : list()) {
        if (process.name == processName) {
            pids.push_back(process.pid);
        }
    }

    return pids;
}

inline int close(const std::string& processName) {
    int closed = 0;

    for (int pid : find(processName)) {
        if (close(pid)) {
            closed++;
        }
    }

    return closed;
}

inline int forceClose(const std::string& processName) {
    int closed = 0;

    for (int pid : find(processName)) {
        if (forceClose(pid)) {
            closed++;
        }
    }

    return closed;
}

}