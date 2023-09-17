## Overview
This section talks about various design patterns/code implemented in this repo
The focus is not always on raw performance until its justified.

#### A single header for user

The database AumDB exposes a single `db.h` as an interface to user and defines all types in here.

#### Easy interface for users
The interface `db.h` exposes functionslity via global function so that user do not have to worry about object instantiation,
lifecycle like copy/move etc. 

Internally, it delegates all work to a singleton engine instance which does manage all state.

#### Result types for query
All result returned by a query is of type `Result` and they are subclasses based on query type.
If a user is interested in only knowing whether a query was succeful(e.g INSERT/DELETE etc) and if failed, whats the 
error, it can just cast to it to avoid indirection overhead. Else it can always cast to specific type.

## Design for Perf

#### Avoid grnaular IO where possible

#### Minimize context switches
Batch system calls(e.g IO) where possible, use mmap for common things.

#### Minimize data copy from kernel to user space
e.g mmap 

