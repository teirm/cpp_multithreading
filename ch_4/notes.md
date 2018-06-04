# Chapter 4: Synchronizing Concurrent Operations 

## Terms

## Includes

***lt&;condition_variablegt&;***
***lt&;futuregt&;***
***lt&;chronogt&;***

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

The structure of a conditional variable is:

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
separate thread
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

The template parameter defines the function 
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
Provides a way to set a value of type T that is 
readable through an associated std::future<T>.
A waiting thread can block on the future while
the providing thread can use the promise to set
the value.

To get the future associated with the std::promise,
use std::promise::get_future().  The future becomes
ready after the value is set in the promise using
std::promise::set_value().  If the std::promise
is destroyed without a value set, an exception
is stored instead.

**Note**: clang++ did *not* throw an error
when compiling a program using promises 
without -lpthread.  The executable then threw a 
std::system_error.

#### Dealing with exceptions in the future
**Note**: std::current_exception retrieves
the currently thrown exception.

Exceptions thrown as part std::async gets
rethrown when std::future::get is called. 
std::promise does the same thing.

**Note**: the standard does not specify if
the exception rethrown is a copy or the 
original.

If a std::packaged_task or a std::promise
is destroyed without setting a value, the
result will be std::future_error.

#### Synchronizing Multiple Threads
Lack of synchronization with std::future and 
multiple threads creates a data race and is
undefined.  Additionally, std::future::get 
is a one shot operation -- after the first call
there is nothing else.  So, the first thread
to make the get() call will get the results.

std::shared_future solves this problem by
being a *copyable* future, whereas std::future
is only *movable*.  Access is still not
synchronized and requires locking; however, the
problem of undefined behavior will not occur.
(The locking problem can also be solved by having
each thread take a copy of the shared future 
instead of referencing the same instance.)

Creating a shared_future from a regular future 
can be done, but invalidates that future. This 
is because the future is moved. For example:
```c++
std::promize<std::string> p;
/* std::move is implicit for rvalues */
std::shared_future<std::string> sf(p.get_future()); 
```

The transfer of ownership can also be accomplished
using std::future::share().

### Timed wait
Timeouts come in two different forms:
* durations - normally have a _for suffix
* absolute  - have a _until suffix

#### Clocks in C++
Clocks in C++ have four pieces of information:
* current time
* time data type
* tick period
* steady tick or not

A note on steady ticking: most clocks are not steady
since steady is defined at ticking at uniform rate and 
*not being adjustable*.  The standard library provides 
std::chrono:steady_clock if necessary.

Additionally, if high precision is needed there is a 
std::chrono::high_resolution_clock.

#### Durations
Given by std::chrono::duration<> template.  The template
specifies the representation and how many seconds each
duration unit represents.

Note conversion of durations is implicit when there is 
no truncation:
* hours -> seconds OK
* seconds -> hours NOT OK

There is a std::chrono::duration_cast<> for the purpose
of explicit conversions; however, the result is truncated
instead of being rounded.

To use a duration wait, use future::wait_for and see if 
after the timeout the result is std::future_status::ready.

#### Time Point
Provided by instances of std::chrono::time_point<>.

Gives the length of time in multiples of a templated
duration since an *epoch* of the clock.  Can be useful
for timing blocks of code.

The clock parameter specifies the epoch of the timepoint
but is also used to determine when the wait should return.
If the parameter is modified, this can impact the total
wait time.

These are used with the _until suffix type waits.  When 
used with a condition variable, a loop should be wrapping
the check for spurious wakeups.  Using wait_for() in this
case can lead to an unbounded wait time if you miss the 
exact time.

#### Alarms
You can make a thread sleep_for() or sleep_until() a 
certain time.

These can also be used with locks to try to acquire a 
lock for a duration.

## Simplifying Code with Synchronization

The techniques described with futures and promises are
very amenable to a functional programming style.

Functional programming is when the function call
depends only on the parameters.  It is a great 
simplification when concurrency in involved.

Futures are critical since they allow for dependencies
between computations without explicit access to shared
data.

Listing 4.12 and 4.14 are good examples of using 
functional C++ style and using futures within this 
style.

### Message Passing for Synchronization
*See the book Communicating sequential process by
C.A.R Hoare*

If there is no shared data, each thread can be 
viewed as independent and as as state machine that 
updates state based upon the messages received and
how they modify the current state.

In C++ all threads share an address space, so true
CSP is not possible.  It requires that there is 
absolutely no shared data.
