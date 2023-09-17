#include <string>
#include <vector>

#include "page.h"

#define MAX_PAGES 80000


namespace AumDb {
    namespace Core {

        using ErrorCode = unsigned char;

        /**
         * There is 1 index file per table which is separate from heap file for data pages.
         * Currently the page size is same for both.
         * Disk format of index file:
         * 
         * [HEADER] [PAGES]
         * 
         * HEADER - [2 byte indicating magic number for header file format, 1 byte indicating version, 4 byte indicating page size, 2 byte
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
        template<typename K>
        class BTreeIndex {
        private:

            struct Header {
                constexpr std::array<unsigned char, 4> MAGIC_NUMBER = {'A', 'u', 'm', 'I'};
                constexpr unsigned char VERSION = 1;

                const size_t page_size_;
                const unsigned short branching_factor_;

                size_t num_pages_;

                size_t root_offset_;

                unsigned char[MAX_PAGES / 8] space_map_;


            }

            unsigned short branchingFactor;  
            unsigned short pageSize;
            std::string file;
            

            struct IndexNode: public Node {
                struct Header {

                    const size_t page_id_;
                    const unsigned short num_pages_;

                }
                std::unique_ptr<K> keys_;
                std::unique_ptr<Node> children_;  
                bool is_leaf_; 

                Node(std::unique_ptr<K> keys, std::unique_ptr<Node> children, bool is_leaf) : keys_(keys), children_(children), is_leaf_(is_leaf) {}

                unsigned char serialize() {

                }
            };

        public:
            // Create from a disk-resident file
            BTreeIndex(std::string& file) {
                
            }

            // Creates a new B-tree.
            // The page size and max key length determine the tree's branching factor.
            BTreeIndex(size_t pageSize, size_t maxKeyLength);

            ~BTreeIndex();

            //---------------------- API Methods

            ErrorCode insert(K key, std::vector<unsigned char> value);

            std::vector<unsigned char> get(K key);

            ErrorCode remove(K key);

            ErrorCode read(size_t pageId, std::shared_ptr<void> at);

            ErrorCode write(size_t pageId, std::shared_ptr<void> from);
        };

    } // namespace Core
} // namespace AumDb
