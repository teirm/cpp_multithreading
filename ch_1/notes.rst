*********
Chapter 1
*********

Terms
*****
**Hardware Concurrency**: multiple cores or processors to run tasks
                          different than **task switching**.

**Hardware Threads**: measure of number of independent tasks that can
                      run concurrently.

**Task Parallelism**: Divide single task into parts to run in parallel

**Data Parallelism**: Divide data and have parallel execution of same task

Includes
********
**<thread>**: provides multithreading support for C++11

Notes
*****

Concurrency can be done with multiple processes; however, there is 
significant overhead:
    * Slow and difficult to set up
    * OS protects modifying data accross processes.
      (This is both good and bad -- see Erlang).

Multithreaded concurrency uses a single process that has separate
threads sharing the same address space.  No overhead to sharing, but
there needs to be added checks:
    * Need to avoid race conditions
    * Atomicity

Concurrency is useful to improve performance and having separation of
work.

Concurrent code is inherently more complex, so avoid it if the costs
outweigh the benefits.  Additionally, threads are not free and can 
exhaust resources.  Though the use of thread pools can alleviate some
of these issues, it is not a complete fix and care must still be taken.

Every additional threads increases the amount of context switching.  Too
much can have a detrimental effect on the program.

clang++ -std=c++11 requires the pthread library linked in with -lpthread 
linked in to use #include <thread>.
