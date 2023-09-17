/*
    A buffer pool is a hashmap from "tableId:i|d:pageID".
    The disk format and memory format of a page need not be same
    and its the responsibility of index to return an in-memory format for disk format
    and vice versa.

    The buffer pool understands the above key but does not understand the contents and delegates to the index or data page
    for actual operations. The index and data file expose simple APIs for it.

    The index and data file expose:
    - page level load/unload API
    - tuple level CRUD

    All page latching is handled by buffer pool.

*/
#include <unordered_map>
#include <string>
#include <mutex>

#include "btree_index.h"
#include "page.h"

namespace AumDb {
    namespace Core {
        
        class BufferManager {
        private:
            const std::string INDEX_SEPARATOR = ":i:";
            size_t page_size_;
            size_t num_pages_;
            std::mutex buffer_mutex_;
            
            // Allocated on demand 
            std::unordered_map<std::string, std::shared_ptr<Page>> buffer_;

        public:

            BufferManager(size_t page_size, size_t num_pages): page_size_(page_size), num_pages_(num_pages) {}

            template<typename T>
            std::shared_ptr<Page> get_index_page(std::string& table_id, std::shared_ptr<AumDb::Core::BTreeIndex<T>> index, size_t page_id) {
                {
                    std::lock_guard lk(buffer_mutex_);
                    if (buffer_.find(table_id + INDEX_SEPARATOR + std::to_string(page_id)) == buffer_.end()) {
                        // allocate page memory, mark page as latched and then delegate loading to index passing memory address


                    }
                
                }

            }            

        }
    } // Core
} // AumDb