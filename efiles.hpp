#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace efiles {

namespace fs = std::filesystem;

// 1. Make one directory
inline bool mkdir(const std::string& path) {
    return fs::create_directory(path);
}

// 2. Make directories recursively
inline bool mkdirs(const std::string& path) {
    return fs::create_directories(path);
}

// 3. Check if path exists
inline bool exists(const std::string& path) {
    return fs::exists(path);
}

// 4. Check if path is a file
inline bool is_file(const std::string& path) {
    return fs::is_regular_file(path);
}

// 5. Check if path is a directory
inline bool is_dir(const std::string& path) {
    return fs::is_directory(path);
}

// 6. Remove one file/directory
inline bool remove(const std::string& path) {
    return fs::remove(path);
}

// 7. Remove directory and everything inside
inline std::uintmax_t remove_all(const std::string& path) {
    return fs::remove_all(path);
}

// 8. Rename something
inline bool rename(
    const std::string& old_path,
    const std::string& new_path
) {
    try {
        fs::rename(old_path, new_path);
        return true;
    } catch (...) {
        return false;
    }
}

// 9. Copy file
inline bool copy(
    const std::string& from,
    const std::string& to
) {
    try {
        fs::copy_file(
            from,
            to,
            fs::copy_options::overwrite_existing
        );

        return true;
    } catch (...) {
        return false;
    }
}

// 10. Move file/directory
inline bool move(
    const std::string& from,
    const std::string& to
) {
    try {
        fs::rename(from, to);
        return true;
    } catch (...) {
        return false;
    }
}

// 11. Read file
inline std::string read(const std::string& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        return "";
    }

    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    );
}

// 12. Write file
inline bool write(
    const std::string& path,
    const std::string& data
) {
    std::ofstream file(path, std::ios::binary);

    if (!file) {
        return false;
    }

    file << data;

    return true;
}

// 13. Append to file
inline bool append(
    const std::string& path,
    const std::string& data
) {
    std::ofstream file(
        path,
        std::ios::binary | std::ios::app
    );

    if (!file) {
        return false;
    }

    file << data;

    return true;
}

// 14. Get file size
inline std::uintmax_t size(const std::string& path) {
    try {
        return fs::file_size(path);
    } catch (...) {
        return 0;
    }
}

// 15. Create empty file if it doesn't exist
inline bool touch(const std::string& path) {
    std::ofstream file(path, std::ios::app);

    return static_cast<bool>(file);
}

// 16. List everything in directory
inline std::vector<std::string> list(
    const std::string& path
) {
    std::vector<std::string> items;

    if (!fs::is_directory(path)) {
        return items;
    }

    for (const auto& entry :
         fs::directory_iterator(path)) {

        items.push_back(
            entry.path().string()
        );
    }

    return items;
}

// 17. List only files
inline std::vector<std::string> list_files(
    const std::string& path
) {
    std::vector<std::string> files;

    if (!fs::is_directory(path)) {
        return files;
    }

    for (const auto& entry :
         fs::directory_iterator(path)) {

        if (entry.is_regular_file()) {
            files.push_back(
                entry.path().string()
            );
        }
    }

    return files;
}

// 18. List only directories
inline std::vector<std::string> list_dirs(
    const std::string& path
) {
    std::vector<std::string> dirs;

    if (!fs::is_directory(path)) {
        return dirs;
    }

    for (const auto& entry :
         fs::directory_iterator(path)) {

        if (entry.is_directory()) {
            dirs.push_back(
                entry.path().string()
            );
        }
    }

    return dirs;
}

// 19. Get extension
inline std::string extension(
    const std::string& path
) {
    return fs::path(path).extension().string();
}

// 20. Get filename
inline std::string filename(
    const std::string& path
) {
    return fs::path(path).filename().string();
}

}