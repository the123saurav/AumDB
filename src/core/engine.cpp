#include <utility>
#include <filesystem>

#include "spdlog/spdlog.h"
#include "hsql/SQLParser.h"

#include "aumdb/core/engine.h"
#include "aumdb/core/catalog.h"
#include "aumdb/core/heap.h"
#include "aumdb/core/index.h"


AumDb::Core::Engine& AumDb::Core::Engine::get_instance() {
    static Engine instance;
    return instance;
}

void AumDb::Core::Engine::start(nlohmann::json config) {
    config_ = config;
    std::string data_dir = config["data_dir"];

    if (AumDb::Core::remove_directory(data_dir)) {
        throw std::runtime_error{"unable to remove data dir"};
    }

    if (AumDb::Core::create_directory(data_dir)) {
        throw std::runtime_error{"unable to create data dir"};
    }

    if (AumDb::Core::Catalog::create_table(data_dir)) {
        throw std::runtime_error{"unable to create table catalog"};
    }
    
    spdlog::info("Engine started");
}

void AumDb::Core::Engine::stop() {
    // TODO
}

AumDb::Result AumDb::Core::Engine::executeQuery(const std::string& query) {
    spdlog::info("Received query: {}", query);

    hsql::SQLParserResult result;
    hsql::SQLParser::parse(query, &result);

    if (result.isValid() && result.size() > 0) {
        spdlog::info("Successfully parsed query");
        assert(result.getStatements().size() > 0);
        auto statement = result.getStatement(0);
        return handleQuery(statement);
    }
   
    spdlog::warn("Bad query: {} with error: {}", query, result.errorMsg());
    return AumDb::Result(AumDb::ErrorCode::BadInput, result.errorMsg());
}

AumDb::Result AumDb::Core::Engine::handleQuery(const hsql::SQLStatement* statement) {
    switch (statement->type()) {
    case hsql::kStmtCreate: {
        const hsql::CreateStatement* create_statement = static_cast<const hsql::CreateStatement*>(statement);

        auto table_detail_or_err = validate_create_table(create_statement);
        
        if (std::holds_alternative<AumDb::Core::ErrorCode>(table_detail_or_err)) {
            auto err_detail = ERROR_MAP.find(std::get<AumDb::Core::ErrorCode>(table_detail_or_err));
            if (err_detail != ERROR_MAP.end()){
               return Result(err_detail->second.first, err_detail->second.second); 
            }
            return Result(AumDb::ErrorCode::RuntimeError, "Unexpected error");
        }

        std::lock_guard lg{write_lock_};

        auto table_detail = std::get<AumDb::Core::TableDetail>(table_detail_or_err);

        if (tables_.find(table_detail) != tables_.end()) {
            return Result(AumDb::ErrorCode::ResourceAlreadyExists, "TableAlreadyExists");
        }
       

        if ( auto ec = AumDb::Core::Catalog::add_table(config_["data_dir"], table_detail); ec) {
            spdlog::error("Create catalog table threw error {}", AumDb::Core::errorcode_to_string(ec.value()));
            return Result(AumDb::ErrorCode::RuntimeError, "catalog table creation error");
        }

        
        if (auto heap_ec = AumDb::Core::HeapFile::create_file(config_["data_dir"], table_detail.name_, config_["page_size"]); heap_ec) {
            spdlog::error("Create heap file for  table threw error {}", AumDb::Core::errorcode_to_string(heap_ec.value()));
            return Result(AumDb::ErrorCode::RuntimeError, "heap file creation error");
        }
        
        if (auto index_ec = AumDb::Core::BTreeIndex<unsigned int>::create_file(config_["data_dir"], table_detail.name_, 
            config_["page_size"], table_detail.pkey_col_size_); index_ec) {
            spdlog::error("Create index file for  table threw error {}", AumDb::Core::errorcode_to_string(index_ec.value()));
            return Result(AumDb::ErrorCode::RuntimeError, "index file creation error");
        }
        

        tables_.insert(table_detail);

        return Result();

    }
    default:
        return Result(AumDb::ErrorCode::BadInput, "Unexpected query");
    }
}

std::variant<AumDb::Core::ColumnType, AumDb::Core::ErrorCode> AumDb::Core::Engine::get_column_type(hsql::DataType dt) {
    switch (dt) {
        case hsql::DataType::INT :
            return AumDb::Core::ColumnType::INT;
        default:
            return AumDb::Core::ErrorCode::UnsupportedColumnType;    
    }
}

std::variant<AumDb::Core::TableDetail, AumDb::Core::ErrorCode> AumDb::Core::Engine::validate_create_table(
    const hsql::CreateStatement* create_table_statement
     ) {
        std::string table_name = create_table_statement->tableName;

        spdlog::info("Table name is: {}", table_name);

        if (table_name.size() == 0) {
            return AumDb::Core::ErrorCode::InvalidSyntax;
        }

        std::vector<AumDb::Core::ColumnDetail> column_details;
        std::unordered_set<std::string> col_names;
        std::string pkey;
        unsigned char pkey_size = 0;

        for (auto& col_def: *(create_table_statement->columns)) {
            std::string col_name = col_def->name;

            if (col_names.insert(col_name).second == false) {
                return AumDb::Core::ErrorCode::DuplicateColumnName;
            }

            auto col_type_or_err = get_column_type(col_def->type.data_type);

            if (std::holds_alternative<AumDb::Core::ErrorCode>(col_type_or_err)) {
                return std::get<AumDb::Core::ErrorCode>(col_type_or_err);
            }
            
            column_details.push_back(
                ColumnDetail(
                    col_name, 
                    std::get<AumDb::Core::ColumnType>(col_type_or_err), 
                    col_def->type.length
                )
            );

            std::unordered_set<hsql::ConstraintType> ss = *(col_def->column_constraints);

            for (auto col_constraint: *(col_def->column_constraints)) {
                if (col_constraint == hsql::ConstraintType::PrimaryKey) {
                    if (pkey.size() > 0) {
                        return AumDb::Core::ErrorCode::DuplicatePrimaryKey;
                    }
                    pkey = col_name;
                    pkey_size = col_def->type.length;
                }
            }
            if (pkey.size() == 0) {
                return AumDb::Core::ErrorCode::NoPrimaryKey;
            }
        }

    return AumDb::Core::TableDetail(std::move(table_name), std::move(pkey), pkey_size, std::move(column_details));
}
