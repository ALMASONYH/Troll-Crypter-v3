#pragma once
#include <string>
#include <fstream>
#include <filesystem>

namespace troll {

class Validator {
public:
    static bool check(const std::string& path, std::string& error) {
        namespace fs = std::filesystem;

        if (!fs::exists(path)) {
            error = "File does not exist";
            return false;
        }

        if (!fs::is_regular_file(path)) {
            error = "Not a regular file";
            return false;
        }

        auto ext = fs::path(path).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext != ".py") {
            error = "Only .py files supported";
            return false;
        }

        auto sz = fs::file_size(path);
        if (sz == 0) {
            error = "File is empty";
            return false;
        }

        if (sz > 50 * 1024 * 1024) {
            error = "File too large (max 50MB)";
            return false;
        }

        std::ifstream f(path);
        if (!f.is_open()) {
            error = "Cannot open file";
            return false;
        }

        return true;
    }
};

}
