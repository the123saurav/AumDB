@startuml


skin rose

title High Level Flow for System boot

participant Client as C
participant Engine as EG
participant SpaceMap as SM
participant WAL
participant BTreeIndex as BT
participant HeapPage as HP
participant BufferManager as BM

C -> EG: start
EG -> EG: create empty buffer pool



