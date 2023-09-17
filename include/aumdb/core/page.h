#include <atomic>

namespace AumDb {
    namespace Core {

        struct Node {
            virtual unsigned char serialize() = 0;
        };

        struct Page {
            bool is_dirty_;
            bool is_modified_;
            std::atomic_bool latch_;    
            // A page owns the node memory
            std::shared_ptr<Node> node;
        }

        

    } // Core
} // AumDb