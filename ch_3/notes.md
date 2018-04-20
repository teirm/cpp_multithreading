# Chapter 3 

## Terms

* Invariant - something that is always true about a data structure

* Race condition - outcome depends on relative ordering of operation
execution.

* Data race - race conditions due to concurrent access of a single object

* Lock Free Programming - style of programming that maintains invariants
without the use of locks.

## Includes

**&lt;mutex&gt;**

**&lt;type_traits&gt;**

## Notes

### Intro

Problems with sharing data are largely a result of modifying the data
through multiple threads.  Read only access is never a problem.  This 
leads to broken invariants.

Race conditions arise when modifications manipulate multiple pieces of
data.  This often requires multiple machine instructions which can 
be interrupted.

Example:

|        Thread 1     |      Thread 2        |
|--------------------------------------------|
|      a = x          |                      |
|                     |    b = x             |
|                     |    x = b + 1         |
|      x = a + 1      |                      |

Very often race conditions are difficult to reproduce and isolate.  To 
avoid race conditions in the first place there are several methods:

* Use synchronization primitives

* Write code to preserve invariants - hard (RCU or lock free programming)

* Handle data transactionally like a database.  There is no support for this
in C++


### Data protection with mutexes

Allow for data structures to be locked, modified, then unlocked.

Mutexes are available in std::mutex and provide lock() and unlock() member 
functions; however, use of std::lock_guard, which provides the RAII idiom
for a mutex - lock on construction, unlock on destruction - is more 
recommended.

It is recommended OOP practice to group data and protection together into
a class.  While protection is provided if all member functions lock and unlock
protected data properly this is *meaningless* if protected data is accessible
by pointer or reference.

To structure code that is safe beyond just using std::mutex, the following
should be ensured:

* Member functions do not pass pointers or references to callers

* Member functions do not pass pointers or references to callees

### Races from interface problems

Take the std::stack interface with the top() and pop() member functions.  In
a multithreaded scenario this can lead to a complicated situation:

```c++
stack<int> f;
if (!f.empty())
{
    int const value = f.top();
    f.pop();
    foo(value);
}
```

In this block, a thread 1 can get false for empty and then go to pop()
but thread 2 could have already pop'd thereby invalidating the result
of empty.

***Note***: Combining top() and pop() into a single call can lead to 
problems with the copy constructor.  If pop() returned the value after
the stack was modified, the modification of the stack could have
thrown a std::bad_alloc exception.  The data popped is lost and the copy
was not successful.  This occurs if there is very little heap memory
available and the copy fails.

Essentially, the item is removed from the stack first, then during when
copying the popped value to the user, the copy fails.  There is no way
to recover the data now.

For more details see:
*Excetional C++: 47 Engineering Puzzles, Programming Problems, and Solutions
by Herb Sutter*

Now,  how to deal with the std::stack interface in a multithreaded context.

* Pass in a reference to get the value.  This requires the stored type 
to be assignable.  If not, they might support std::move.

* Require no throw copy constructor or move constructor.  Exception 
safety is only a problem if the return by value throws.  It is not 
ideal, but the existence of such constructors is detectable at
compile time using std::is_nothrow_copy_constructible and
std::is_nothrow_move_constructible.

* Return a pointer to the popped item.  Shared pointer can be used
to avoid issues with memory management and it avoids the above problem.

* Provide option 1 and 2 or 3.

***Note***: The discussion regarding pop(), top(), memory allocations,
and exception safety is to highlight the problems that arise from 
combining pop() and top().  This needs to be done in a multithreaded
context.

Fundamentally, the issue is lock granularity: locks needs to capture
the entire operation or operations that can modify the data.  However,
locking at too large a granularity leads to miserable performance due
to lock contention.

## Deadlock
**From Modern Operating Systems Ch 6**:
** The conditions for deadlock are:
* mutual exclusion - each resource is either assigned or available
* Hold and wait condition
* No preemption - resources must be explicitly released
* Circular wait
**

Best advice is to always lock in the same order.  C++ provides a fix
through std::lock -- lock two or more mutexes at once.

Additionally, std::lock_guard can be told to "adopt" the current lock
-- do not lock it, but take it in its current state -- by using
the argument std::adopt_lock.  std::lock is an all or nothing.

### Deadlock avoidance guidelines

Fundamentally, don't wait on another thread if it is possible waiting
for you.

More drastically the following should be considered:
* Never acquire more than 1 lock

* Avoid calling user code when holding a lock

* Acquire locks in a fixed order -- define an order of traversal.

* Use a locking hierarchy -- divide locks into layers and prevent locking if 
a lock from a lower layer is already held

Through the use of thread_local variables, a hierarchical mutex is easy to
implement.  The hierarchy values are stored within the lock as they are 
acquired and are all thread local.
