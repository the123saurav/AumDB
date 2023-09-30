#include <fstream>
#include <vector>
#include <type_traits>

#include "CRC.h"
#include "spdlog/spdlog.h"
#include "aumdb/core/catalog.h"
#include "aumdb/core/util.h"

namespace fs = std::filesystem;

/**
There could be multiple catalog tables in the system.
Each catalog table is a self contained file on disk.
This file deals with serialization from a UDT to disk and deserialization to it from disk.
At the moment, we start with only following:

1. aumdb_tables

This is of form:

<HEADER><ENTRIES>

<HEADER> 4kb reserved: 

<4 byte for format><1 byte for version><num_entries>

<ENTRY> is for format:

<entry_checksum><length prefixed table_name><col_name1, col_type, col_length><col_name2, col_type, col_length><pkey col index>
4              +     (2 + N)               +  (2 + N + 1 + 2)                                                +  2
In future we will persist column in separate catalog tables. 

Reader should first verify checksum for entry before operating on it when reading from disk.

Given that we only allow create, we will just use an append only order for entries.

Note that a DDL is always a self contained Tx and we would not allow it to be within a user Tx
which will only have DML.
*/
namespace AumDb {
    namespace Core {
        namespace Catalog {
            
            struct TableHeader {
                static constexpr std::array<unsigned char, 4> MAGIC_NUMBER = {'A', 'm', 'C', 't'};
                static constexpr unsigned char VERSION = 1;
                // unsigned short num_entries_;
            };

            struct TableEntry {
                const unsigned int checksum_;
                const std::string table_name_;

                const std::vector<ColumnDetail> column_details_;
                const std::string pkey_;

                TableEntry(unsigned int checksum, const std::string& table_name,
                        const std::vector<ColumnDetail>& column_details, const std::string& pkey)
                    : checksum_(checksum),
                    table_name_(table_name),
                    column_details_(column_details),
                    pkey_(pkey)
                {}
            };
        }
    }
}

AumDb::Core::ColumnDetail::ColumnDetail(std::string col_name, AumDb::Core::ColumnType col_type, AumDb::Core::COL_SIZE col_size):
    col_name_(col_name), col_type_(col_type), col_size_(col_size) {}
    

AumDb::Core::TableDetail::TableDetail(std::string name, std::string pkey, unsigned char pkey_col_size, std::vector<AumDb::Core::ColumnDetail> columns):
 name_(name), pkey_col_name_(pkey), pkey_col_size_(pkey_col_size), column_details_(columns) {}

bool AumDb::Core::TableDetail::operator==(const AumDb::Core::TableDetail& other) const {
    return name_ == other.name_;
}

std::optional<AumDb::Core::ErrorCode> AumDb::Core::Catalog::create_table(const std::string& dir) {
    std::string system_dir = dir + "/" + AumDb::Core::Catalog::SYSTEM_DIR + "/"  ;
    
    if (auto ec = AumDb::Core::create_directory(system_dir); ec) {
        return ec;
    }

    try {
        spdlog::info("Creating catalog table aumdb_tables");
        std::ofstream table_catalog_stream(system_dir + "aumdb_tables.tmp", std::ios::binary);

        // Headers are always 4KB to have page aligned and also gives enough buffer
        std::vector<unsigned char> header(4096);

        std::copy(AumDb::Core::Catalog::TableHeader::MAGIC_NUMBER.begin(),
            AumDb::Core::Catalog::TableHeader::MAGIC_NUMBER.end(), header.begin());

        header[4] = AumDb::Core::Catalog::TableHeader::VERSION;

        // Write header to the file
        table_catalog_stream.write(reinterpret_cast<const char*>(header.data()), header.size());

        table_catalog_stream.flush();
        table_catalog_stream.close();

        std::filesystem::rename(system_dir + "aumdb_tables.tmp", system_dir + "aumdb_tables");
        spdlog::info("Successfully created catalog table aumdb_tables");
        return {};
    } catch (std::exception& e) {
        spdlog::error("Error in creating catalog table: {}", e.what());
        return AumDb::Core::ErrorCode::IoError;
    }
}


std::optional<AumDb::Core::ErrorCode> AumDb::Core::Catalog::add_table(const std::string& dir, 
    const AumDb::Core::TableDetail& table_detail) {
        
    std::vector<unsigned char> entry(4);

    AumDb::Core::write_integer(entry, entry.size(), static_cast<unsigned short>(table_detail.name_.size()));
    
    for (auto& ch: table_detail.name_) {
     entry.push_back(ch);
    } 

    unsigned short pkey_index = -1;

    for(int i=0; i < table_detail.column_details_.size(); ++i) {
        std::string col_name = table_detail.column_details_[i].col_name_ ;
        if (col_name == table_detail.pkey_col_name_) {
            pkey_index = i;
        }
        AumDb::Core::write_integer(entry, entry.size(), static_cast<unsigned short>(col_name.size()));
        for (auto& ch: col_name) {
            entry.push_back(ch);
        }

        AumDb::Core::write_integer(entry, entry.size(), static_cast<unsigned char>(table_detail.column_details_[i].col_type_));
        AumDb::Core::write_integer(entry, entry.size(), static_cast<unsigned short>(table_detail.column_details_[i].col_size_));
    }

    AumDb::Core::write_integer(entry, entry.size(), static_cast<unsigned short>(pkey_index));

    std::uint32_t crc = CRC::Calculate(entry.data(), entry.size(), CRC::CRC_32());

    AumDb::Core::write_integer(entry, 0, crc);

    return AumDb::Core::append_to_file(dir + "/" + SYSTEM_DIR + "/aumdb_tables", entry);
}




