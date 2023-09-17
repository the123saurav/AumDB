#ifndef AUMDB_CORE_CATALOG
#define AUMDB_CORE_CATALOG

#include <string>
#include <unordered_set>
#include "aumdb/core/util.h"

namespace AumDb {
    namespace Core {

        using COL_SIZE = unsigned short;

        enum class ColumnType: uint8_t {
            INT = 0
        };

        struct ColumnDetail {
            std::string col_name_;
            ColumnType col_type_;
            COL_SIZE col_size_;

            ColumnDetail(std::string col_name, ColumnType col_type, COL_SIZE col_size);

            // bool operator==(const ColumnDetail& other);
        };

        struct TableDetail {
            std::string name_;
            std::string pkey_col_name_;
            unsigned char pkey_col_size_;
            std::vector<ColumnDetail> column_details_;

            TableDetail(std::string name, std::string pkey_col_name, unsigned char pkey_col_size, std::vector<ColumnDetail> column_details);

            bool operator==(const TableDetail& other) const;
        };

        namespace Catalog {
            const std::string SYSTEM_DIR = "system";

            std::optional<AumDb::Core::ErrorCode> create_table(const std::string& dir);

            std::optional<AumDb::Core::ErrorCode> add_table(const std::string& dir, const TableDetail& table_detail);
        };

    } // Core    
} // AumDb 

#endif