#include <vector>
#include <array>
#include <optional>

#include "aumdb/core/page.h"
#include "aumdb/core/util.h"

/**
 *

 This is of form:

<HEADER><PAGES>

<HEADER> 4kb reserved:

<4 byte for format><1 byte for version>

<ENTRY> is for format:

<entry_checksum><binary>
4              +  

*/

namespace AumDb
{
    namespace Core
    {

        struct HeapFile
        {

            struct Header
            {
                static constexpr std::array<unsigned char, 4> MAGIC_NUMBER = {'A', 'm', 'H', 'p'};
                static constexpr unsigned char VERSION = 1;

                const PAGE_SIZE_T page_size_;
                PAGE_SIZE_T num_pages_;

                unsigned char space_map_0_33_[MAX_PAGES / 8];
                unsigned char space_map_33_66_[MAX_PAGES / 8];
                unsigned char space_map_66_[MAX_PAGES / 8];
            };

            struct Page : AumDb::Core::Node
            {

                struct Tuple
                {
                    unsigned int checksum_;
                    // The fields size is inferred from catalog schema for the table
                    // owning the page. For variable types, first X bytes based on field
                    // length specified in catalog is read to find the actual length of data.
                    std::vector<unsigned char> data_;

                    Tuple(PAGE_SIZE_T checksum, std::vector<unsigned char> data) : checksum_(checksum), data_(data) {}
                };

                PAGE_SIZE_T free_space_;

                PAGE_SIZE_T page_lsn_;

                std::vector<Tuple> slots_;
            };

            static std::optional<AumDb::Core::ErrorCode> create_file(const std::string & dir, const std::string & table_name,
                PAGE_SIZE_T page_size);
        };
    }
}