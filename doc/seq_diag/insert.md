@startuml


skin rose

title High Level Flow for Insert

participant Client as C
participant Engine as EG
participant SpaceMap as SM
participant WAL
participant BTreeIndex as BT
participant HeapPage as HP
participant BufferManager as BM


C -> EG: execute query
EG -> EG: take WLock on table/index
EG -> EG: get root page id from catalog
EG -> EG: resolve the BT
EG -> BT: insert tuple(root page)

loop discover leaf Page
  BT -> BM: get page(id, read)
  BM -> BM: lock the pool for read
  
  group under RLock
    BM -> BM: locate the page in pool
    alt page not exists in pool
      BM -> BM: upgrade to WLock
      alt no memory for page
        BM -> BM: evict non dirty LRU page, if needed checkpoint
      end
      BM -> BM: create a new uninitialized page with pageid
      BM -> BM: latch page for "write" and mark dirty
    end 
  end group
  
  BM -> BT: return page, flag to indicate its not loaded and unlatch callback, flag for insert will split, and add page to traversal map
  
  alt page needs loading
    BT -> BT: load page to specified memory
    BT -> BT: fetch child page id if not leaf
    BT -> BT: unlatch callback
  end
end

alt tuple exists in leaf
  BT -> EG: error{already_exists}
else tuple not exists
  BT -> EG: return leaf page
end  

EG -> BM: latch page for write
EG -> BT: check if insert would lead to split in leaf

alt insert leads to split
  EG -> BM: locate ancestor in traversal map and follow load page above if not exists in pool 
  EG -> BT: split the tree starting at pageId to traversal map
end

EG -> SM: get data page with space
alt no "right" free page
  SM -> HP: allocate new page
  HP -> HP: grow file
  HP -> HP: update file header
  HP -> SM: return page
  SM -> SM: update its view
end
SM -> EG: return right size free page
EG -> HP: insert(new leaf page, tuple)

HP -> BM: get page(id, write)
BM -> BM: lock the pool for read
group under RLock
  alt page not in pool
    BM -> BM: upgrade to WLock
    alt no memory for page
        BM -> BM: evict non dirty LRU page, if needed checkpoint
    end
    BM -> BM: create a new uninitialized page with pageid
  end
end
BM -> BM: latch page for "write" and mark dirty

BM -> HP: return page, flag to indicate its not loaded and unlatch callback

alt page needs loading
    HP -> HP: load page to specified memory
end

HP -> EG: return data page
EG -> HP: add tuple(tuple) to page
HP -> BM: unlatch page
EG -> C: ok




@enduml