#ifndef AUMDB_CORE_HASH_DEF_H
#define AUMDB_CORE_HASH_DEF_H

#include "aumdb/core/catalog.h"

namespace std {
    // template<> struct hash<AumDb::Core::ColumnDetail> {
    //     size_t operator()(const AumDb::Core::ColumnDetail& col) const {
    //         size_t h1 = std::hash<std::string>()(col.col_name_);
    //         size_t h2 = std::hash<int>()(static_cast<int>(col.col_type_));  // Assuming ColumnType is an enum
    //         size_t h3 = std::hash<size_t>()(col.col_size_);  // Assuming COL_SIZE is a typedef for size_t
    //         return h1 ^ (h2 << 1) ^ (h3 << 2);
    //     }
    // };

    template<> 
    struct hash<AumDb::Core::TableDetail> {
        using argument_type = AumDb::Core::TableDetail;
        using result_type = size_t;
        
        result_type operator()(argument_type const& table) const noexcept {
            return std::hash<std::string>()(table.name_);
        }
    };
}

#endif