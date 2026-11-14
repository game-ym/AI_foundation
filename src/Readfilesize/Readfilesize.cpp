#include "AI/Readfilesize.h"
#include <iostream>
size_t GetFileSizeMB(const std::string& filepath) {
    try {
        auto bytes = std::filesystem::file_size(filepath);
        return bytes / (1024 * 1024);
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error: File not found" << filepath << std::endl;
        return 0;
    }
}