@startuml


skin rose

title High Level Flow for Create Table

participant Client as C
participant Engine as EG
participant SpaceMap as SM
participant WAL
participant BTreeIndex as BT
participant HeapPage as HP
participant BufferManager as BM

C -> EG: execute query 
EG -> EG: validate query for create
EG -> EG: take in memory table writer lock

EG -> EG: validate no entry in catalog
EG -> HP: create heap page structure on disk
EG -> BT: create BTree structure on disk
EG -> SM: initialize SM

EG -> EG: create entry in catalog


