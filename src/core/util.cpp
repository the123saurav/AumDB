#include <fstream>

#include "spdlog/spdlog.h"

#include "aumdb/core/util.h"



std::optional<AumDb::Core::ErrorCode> AumDb::Core::append_to_file(const std::string& filename, const std::vector<unsigned char>& data) {
    std::ofstream file;

    file.open(filename, std::ios::app | std::ios::binary);

    if (!file.is_open()) {
        spdlog::error("Failed to open the file {} for appending", filename);
        return AumDb::Core::ErrorCode::IoError;
    }

    file.write(reinterpret_cast<const char*>(data.data()), data.size());

    return {};
}

std::optional<AumDb::Core::ErrorCode> AumDb::Core::create_directory(const std::filesystem::path& path) {
    if (!std::filesystem::exists(path)) {
        try {
            std::filesystem::create_directories(path);
            spdlog::info("Created dir {}", path.c_str());
            return {};
        } catch (const std::filesystem::filesystem_error& e) {
            spdlog::error("Error in creating dir {}, error: {}", path.c_str(), e.what());
            return AumDb::Core::ErrorCode::IoError;
        }
    }
    return {};
}


std::optional<AumDb::Core::ErrorCode> AumDb::Core::remove_directory(const std::filesystem::path& path) {
    try {
        std::filesystem::remove_all(path);
        spdlog::info("Removed dir {}", path.c_str());
        return {};
    } catch (const std::filesystem::filesystem_error& e) {
        spdlog::error("Error in removing dir {}, error: {}", path.c_str(), e.what());
        return AumDb::Core::ErrorCode::IoError;
    }
}