#ifndef AUMDB_CORE_UTIL_H
#define AUMDB_CORE_UTIL_H

#include <vector>
#include <string>
#include <optional>

#include "spdlog/spdlog.h"

namespace AumDb {
    namespace Core {

        using PAGE_SIZE_T = unsigned short;
        constexpr unsigned short MAX_PAGES = 8000;
        constexpr unsigned char MAX_COLUMN_LENGTH = 255;

        // Enums for internal error codes
        enum class ErrorCode {
            None,
            InvalidSyntax,
            DuplicateColumnName,
            DuplicatePrimaryKey,
            NoPrimaryKey,
            UnsupportedColumnType,
            TableAlreadyExists,

            IoError
        };

        inline std::string errorcode_to_string(ErrorCode er) {
            switch(er) {
                case ErrorCode::None : return "None";
                case ErrorCode::InvalidSyntax : return "InvalidSyntax";
                case ErrorCode::DuplicateColumnName : return "DuplicateColumnName";
                case ErrorCode::DuplicatePrimaryKey : return "DuplicatePrimaryKey";
                case ErrorCode::UnsupportedColumnType : return "UnsupportedColumnType";
                case ErrorCode::TableAlreadyExists : return "TableAlreadyExists";
                case ErrorCode::IoError : return "IoError";
                default: throw std::runtime_error("Invalid Error");
            }
        }

        template<typename T>
        void write_integer(std::vector<unsigned char>& bytes, size_t offset, T value) noexcept {
            static_assert(std::is_integral<T>::value, "write_integer can only be used with integral types");

            if (offset + sizeof(value) > bytes.size()) {
                bytes.resize(offset + sizeof(value));
            }

            for (size_t i = 0; i < sizeof(value); ++i) {
                bytes[offset + i] = (value >> (8 * i)) & 0xFF;
            }
        }

        std::optional<AumDb::Core::ErrorCode> create_directory(const std::filesystem::path& path);
        
        std::optional<AumDb::Core::ErrorCode> remove_directory(const std::filesystem::path& path);

        std::optional<ErrorCode> append_to_file(const std::string& filename, const std::vector<unsigned char>& data);

    } // Core
    
} // AumDb 

#endif