# Advanced-Memory-Allocator
A realistic memory allocator.

## Part 1 - Understand mmap
The first thing to know is that brk (or the sbrk() function) is not the only way to request memory. Unix provides the mmap system call as well, which allows us to directly map into memory. There is some overhead in creating this mapping, but once it is created, there is no need for a context switch from the kernel, because the memory is directly mapped to where it needs to go. That is fantastic news for us! The only other major constraint is that allocations with mmap must be made in multiples the size of a page(If you specify this incorrctly, mmap will round up). This means if our page size is 4kb (4096 bytes), and we only use 12 bytes in our program, we have some waste!

## Part 2 - Multi-threading

In a multi-threaded environment, we cannot simply make requests to our 'malloc' and 'free' functions. We could certainly get an unlucky interleaving of instructions. Lucklily, we have mutexes to enforce mutual exlusion, and thus create a critical section that is executed sequentially when we are performing operations over shared resources.

**What you must do** Have some locking mechanism that takes place whenever their is an allocation or free.

## Part 3 - Multiple free lists

Depending on the number of cores a computer has, it is often optimal to have seperate free lists per cpu. Then, as separate threads run, they can be associated with locks on a per-cpu basis that the thread is running on. Investigate `man sched_getcpu`. You might additionally investigate `nproc`, `lscpu`,`man get_nprocs()`, and `man get_nprocs_conf()` to figure out where these commands may be useful.

**What you must do** Have multiple free lists created based on the number of cpus. Make sure to update your locking mechanism to make sure you have enough locks per cpu.

## Part 4 - Understanding a pool allocator

A pool allocator, allocates a set amount of memory optimized for some workload. A common way to have a 'pool' of allocated blocks would be to have a fixed set of blocks.

<img align="middle" src="https://3.bp.blogspot.com/-bzESt0SePCg/VriOTcYStBI/AAAAAAAAAIQ/wxPV4jCV644/s640/Segregated%2BFree%2BList.png" width="300px">

Here a free list is initialized with several blocks that are all the same size, one after the other.
