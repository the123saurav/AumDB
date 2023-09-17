#ifndef AUMDB_CORE_ENGINE_H
#define AUMDB_CORE_ENGINE_H

#include <variant>
#include <unordered_map>
#include <string>
#include <mutex>

#include "nlohmann/json.hpp"

#include "hsql/SQLParser.h"

#include "aumdb/core/hash_defintion.h"
#include "aumdb/core/util.h"
#include "aumdb/db.h"
#include "aumdb/core/page.h"
#include "aumdb/core/catalog.h"


namespace AumDb {
    namespace Core {

        struct Engine {

        private:
            const std::unordered_map<AumDb::Core::ErrorCode, std::pair<AumDb::ErrorCode, std::string>> ERROR_MAP = {
                { AumDb::Core::ErrorCode::None, std::make_pair(AumDb::ErrorCode::BadInput, "None") },
                
                { AumDb::Core::ErrorCode::InvalidSyntax, std::make_pair(AumDb::ErrorCode::BadInput, "InvalidSyntax") },
                { AumDb::Core::ErrorCode::DuplicateColumnName, std::make_pair(AumDb::ErrorCode::BadInput, "DuplicateColumnName") },
                { AumDb::Core::ErrorCode::DuplicatePrimaryKey, std::make_pair(AumDb::ErrorCode::BadInput, "DuplicatePrimaryKey") },
                { AumDb::Core::ErrorCode::NoPrimaryKey, std::make_pair(AumDb::ErrorCode::BadInput, "NoPrimaryKey") },
                { AumDb::Core::ErrorCode::UnsupportedColumnType, std::make_pair(AumDb::ErrorCode::BadInput, "UnsupportedColumnType") },
                { AumDb::Core::ErrorCode::TableAlreadyExists, std::make_pair(AumDb::ErrorCode::BadInput, "TableAlreadyExists") },

                { AumDb::Core::ErrorCode::IoError, std::make_pair(AumDb::ErrorCode::RuntimeError, "IoError") },
            };

            nlohmann::json config_;

            std::mutex write_lock_;

            std::unordered_set<AumDb::Core::TableDetail> tables_;

            Engine() {}  // private constructor
            
            Engine(const Engine&) = delete;  // non-copyable
            
            Engine& operator=(const Engine&) = delete;  // non-assignable

            AumDb::Result handleQuery(const hsql::SQLStatement* statement);

            std::variant<AumDb::Core::ColumnType, AumDb::Core::ErrorCode> get_column_type(hsql::DataType dt);

            std::variant<AumDb::Core::TableDetail, AumDb::Core::ErrorCode> validate_create_table(
                const hsql::CreateStatement* create_table_statement);

        public:    
        // A Page can be index internal page, index leaf page and data page
            std::unordered_map<std::string, std::shared_ptr<AumDb::Core::Page>> buffer_pool_;

            static Engine& get_instance();

            void start(nlohmann::json config);

            void stop();

            AumDb::Result executeQuery(const std::string& query);
        };
        
    } // Core
} // AumDb

#endif