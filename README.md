# AumDB

[![Build](https://github.com/the123saurav/AumDB/actions/workflows/cmake.yml/badge.svg)](https://github.com/the123saurav/AumDB/actions/workflows/cmake.yml)

A relational, embedded database for educational purposes

# P0 

List of features would be:

* CREATE TABLE with datatypes INT and SINGLE COLUMN PKEY.
* SINGLE item INSERT 
* SELECT *
* Query planner:
  * Full table scan


## Code Guideline
- Stick to c++ guidelines
- Prefer error codes in place of exception.
- Avoid raw pointer
- Avoid unnecessary copy, prefer pass by ref or std::move

## Design Notes

### In memory data structures
Space map
Active Tx
Buffer Pool
Modified Pages


### Data Page Layout

A file per table which in header tracks few things.
In P0, the space map(what pages available and how much percent free) in memory.
This data structure is created at boot by reading all pages slots info at boot.
A page is fixed size.

In space map, we have 3 categories tracked in P0:
- less than 33% full
- 33% - 66% full
- >66% full

A page would be configurable by user.
It would be slotted page:
- header
- slots
- tuples

The index leaf pages have (pageId, slot) which allows for indirection within page like compation etc.

A page is self contained.


### Index

#### B+Tree Index
