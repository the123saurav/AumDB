#include <string>
#include <vector>
#include <fstream>

#include "page.h"
#include "util.h"

namespace AumDb
{
    namespace Core
    {


        /**
         * There is 1 index file per table which is separate from heap file for data pages.
         * Currently the page size is same for both.
         * Disk format of index file:
         *
         * [HEADER] [PAGES]
         *
         * HEADER - [4 byte indicating magic number for header file format, 1 byte indicating version, 4 byte indicating page size, 2 byte
         * indcating branching factor, number of pages, root page offset, DYNAMIC space bitmap [fixed LIMIT, [bitmap of num pages] next spacemap offset] ]
         * [X reserved bytes upto root page offset].
         * Root page offset is also stored in catalog aum_tables for fast access as its fixed for lifetime.
         * So spacemap is a linked list of bitmaps. Initially, we will have fixed size bitmap say 10k pages.
         *
         * A page has also got header:
         * [pageId, numEntries]
         * The page is not slotted page.
         * So it has just got keys starting from top as:
         * [child1][len1][key1][child2][len2][key2][child3][len3][key3][child4].....
         *
         * The disk represnetation is just a dump as ordering it with every insert/delete would be costly.
         * Root page always has id 0
         *
         */
        template <typename K>
        class BTreeIndex
        {
        private:
            struct Header
            {
                static constexpr std::array<unsigned char, 4> MAGIC_NUMBER = {'A', 'm', 'I', 'x'};
                static constexpr unsigned char VERSION = 1;

                const size_t page_size_;
                const unsigned short branching_factor_;

                size_t num_pages_;
            };

            unsigned short branchingFactor;

            struct IndexNode : public Node
            { 
                std::unique_ptr<K> keys_;
                // page id of child
                std::unique_ptr<size_t> children_;

                bool is_leaf_;

                IndexNode(std::unique_ptr<K> keys, std::unique_ptr<size_t> children, bool is_leaf) : keys_(keys), children_(std::move(children)), is_leaf_(is_leaf) {}

                unsigned char serialize()
                {
                }
            };

        public:
            static std::optional<AumDb::Core::ErrorCode> create_file(const std::string &dir, const std::string &table_name,
                                                                     PAGE_SIZE_T page_size,
                                                                     unsigned char max_pk_length)
            {
                std::string table_dir = dir + "/" + table_name + "/";
                
                if (auto ec = AumDb::Core::create_directory(table_dir); ec)
                {
                    return ec;
                }
                try
                {
                    spdlog::info("Creating index file for {}", table_name);
                    std::ofstream index_stream(table_dir + "index.tmp", std::ios::binary);

                    /*
                        Calculate branching factor now.
                        page_size - 1(leaf) = (max_primarykey_len + 1)* bf + 2 * (bf + 1)
                    */
                   PAGE_SIZE_T bf = (page_size - 1 - 2)/(max_pk_length + 3);
                   spdlog::info("Branching factor calculated as {}", bf);

                    // Headers are always 4KB to have page aligned and also gives enough buffer
                    std::vector<unsigned char> header(4096);
                    {
                        std::copy(Header::MAGIC_NUMBER.begin(),
                                  Header::MAGIC_NUMBER.end(), header.begin());

                        header[4] = Header::VERSION;

                        AumDb::Core::write_integer(header, 5, page_size);
                        AumDb::Core::write_integer(header, 7, bf);
                        AumDb::Core::write_integer(header, 9, 0);
                    }

                    // Write header to the file
                    index_stream.write(reinterpret_cast<const char *>(header.data()), header.size());

                    index_stream.flush();
                    index_stream.close();

                    std::filesystem::rename(table_dir + "index.tmp", table_dir + "index");
                    spdlog::info("Successfully created index table for table {}", table_name);
                    return {};
                }
                catch (std::exception &e)
                {
                    spdlog::error("Error in creating index table: {}", e.what());
                    return AumDb::Core::ErrorCode::IoError;
                }
            }

            //---------------------- API Methods

            // ErrorCode insert(K key, std::vector<unsigned char> value);

            // std::vector<unsigned char> get(K key);

            // ErrorCode remove(K key);

            // ErrorCode read(size_t pageId, std::shared_ptr<void> at);

            // ErrorCode write(size_t pageId, std::shared_ptr<void> from);
        };

    } // namespace Core
} // namespace AumDb
