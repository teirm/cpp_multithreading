# Chapter 4: Synchronizing Concurrent Operations 

## Terms

## Includes

***lt&;condition_variablegt&;***
***lt&;futuregt&;***

## Notes

The previous chapter discussed the protection of data 
using locks; however, threads might also need to 
wait on one another prior to performing actions.

C++11 provides features for thread synchronization using
*condition variables* and *futures*.

### Waiting for Events

Constantly checking for a "completed flag" is bad.
* Need to repeatedly check
* Waiting thread blocks all other threads -- others have
to also constantly check

Sleeping between locks and unlocks is also tricky.
* Better, but the period is difficult to get
* Too short -- waste time checking
* Too long -- dropped tasks or over-running a time slice

Best route is to use condition variables.  Threads can
wait for the condition to be satisfied.  When a thread 
finds the condition satisfied, it can notify other threads
and wake them up.

#### Condition Variables

C++11 has two implementations of condition variables.
Both require a mutex for synchronization. 
* std::condition_variable - only works with std::mutex
* std::condition_variable_any - works with something meeting
the criteria for being *mutex like*.

std::condition_variable_any, being more flexible, incurs 
slightly higher resource costs than std::condition_variable.

***Aside:*** The **explicit** keyword forces the constructor
to be called and won't create the object otherwise.

The structure of a conditiona variable is:

```c++
mutable std::mutex mut;
std::condition_variable my_cond;

std::unique_lock<std::mutex> lk(mut);
my_cond.wait(lk, []{ return !empty(); });
```

condition_variable::notify_one will signal one thread to
check its condition.  The choice of thread is not 
deterministic.  If many threads are waiting for a condition,
condition_variable::notify_all can be used.

If the waiting thread is only waiting for work once, a 
**future** might be a better idea than a condition 
variable.

#### Futures
How to wait for one-off events. A future may have data
or not and the waiting thread can periodically check it
or do other work until the event needs to have happened.

Once a future is read it can't be reset.

There are two types of futures:
* std::future<> - a unique future.  The uniqueness is 
with respect to the associated event.
* std::shared_future<> - a shared future.  Allows for 
multiple instances of the same event.  All become
ready together.

Future objects do not provide synchronization.  They
exist purely as a means for inter thread communication.

A common application is to return a value from a 
background task.

It is up to the implementation how std::async tasks
run.  They can be in a separate thread or run
synchronously when waited on. This can be controlled
with additional arguments:
* std::launch:async - forces the task to be run in a
seprate thread
* std::launch::deferred - call is deferred until 
wait() or get() is called on the future.

The | (logical or) of these two allows the 
implementation to decide and is the default.

#### Packaged Task
std::packaged_task<> ties a future to a function 
or callable object.

When invoked, it will call the associated callable
and makes the future ready.  The return value is
the associated data for the future.

The template paramter defines the function 
associated with the packaged_task.

```c++
/* The callable takes a std::string& and
 * a double*.  int is the type of 
 * std::future returned.
 */
std::packaged_task<int(std::string&, double*)>
```

An example of how this can be used is covered in the
book.  This can be viewed as handing work to another
thread in the form of a std::packaged_task.  The 
caller gets the associated std::future.  This allows
the work to be done in another thread and lets the
caller get the information when the future is ready.
Alternatively, the caller can just ignore the future.

#### Promises
