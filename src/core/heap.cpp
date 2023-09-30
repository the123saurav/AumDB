#include <fstream>

#include "aumdb/core/heap.h"

namespace AumDb
{
    namespace Core
    {
        std::optional<AumDb::Core::ErrorCode> HeapFile::create_file(const std::string &dir, 
            const std::string &table_name,
            PAGE_SIZE_T page_size)
        {
            std::string table_dir = dir + "/" + table_name + "/";
            if (auto ec = AumDb::Core::create_directory(table_dir); ec)
            {
                return ec;
            }
            try
            {
                spdlog::info("Creating heap table for {}", table_name);
                std::ofstream heap_stream(table_dir + "heap.tmp", std::ios::binary);

                // Headers are always 4KB to have page aligned and also gives enough buffer
                std::vector<unsigned char> header(4096);
                {
                    std::copy(HeapFile::Header::MAGIC_NUMBER.begin(),
                              HeapFile::Header::MAGIC_NUMBER.end(), header.begin());

                    header[4] = HeapFile::Header::VERSION;

                    AumDb::Core::write_integer(header, 5, page_size);
                    AumDb::Core::write_integer(header, 7, static_cast<PAGE_SIZE_T>(0));

                    // space maps and beyond are zeroed
                    std::fill(header.begin() + 9, header.end(), 0);
                }

                // Write header to the file
                heap_stream.write(reinterpret_cast<const char *>(header.data()), header.size());

                heap_stream.flush();
                heap_stream.close();

                std::filesystem::rename(table_dir + "heap.tmp", table_dir + "heap");
                spdlog::info("Successfully created heap table for table {}", table_name);
                return {};
            }
            catch (std::exception &e)
            {
                spdlog::error("Error in creating heap table: {}", e.what());
                return AumDb::Core::ErrorCode::IoError;
            }
        }
    }
}
