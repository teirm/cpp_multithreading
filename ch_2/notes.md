# Chapter 2

## Terms

* Copyable
* Movable
* Over subscription
* Default Constructible 

### Includes

* **&lt;functional&gt;**
* **&lt;iterator&gt;**

## Notes

Basic operations can be managed through the std::thread object.

Launching a thread can be done with just a void function or
a function object.  In fact, the std::thread can be created
using any callable object.

```cpp
class foo
{
    public:
        void operator() () const
        {
            bar();
            choo();
        }
};
foo my_foo;
std::thread foo_thread(my_foo);

```
The creation of foo_thread invokes the copy constructor of foo
to foo_thread a memory copy of my_foo.

Care needs to be maintained to not **call** my_foo when creating
foo_thread.  You can't pass a temporary to std::thread -- otherwise
it gets called.  What happens is a function is created that takes 
an argument and returns a std::thread.

C++11 provides ways to avoid this:

```cpp
std::thread foo_thread((foo()));
std::thread foo_thread{foo()};

// you can also start a thread with a lambda
std::thread foo_thread([] () {
        stuff();
    });
```

Care needs to be taken that thread lifetime is not longer
than dependent variables. This includes pointers and references.
This can be avoiding in two ways:
* Copy the variables so that the thread is self-contained.
* Join on the thread so that it will not exit before the thread
  finishes.

join() prevents other useful work from being done by the parent thread.
It will either finish or wait.  A thread can only be "joined on" once.
All subsequent joins will return false a value that can be checked by 
joinable().

**Warning**: join() needs to be handled carefully in exceptional cases.
If not, lifetimes can change leading to incorrect behavior.  Depending
on the structure, join() may need to be called in the exceptional 
(catch) and in the non-exceptional cases.

RAII can be used for this to create a class that perform the join()
on destruction.  However, functions may need to be marked as =delete
to avoid being synthesized by the compiler.

This is only if there is a need to join().  detatch() breaks the 
std::terminate() association because it puts the thread into the
background. detatch() can only be used if std::joinable() returns
true.

### Passing Thread Arguments
```cpp
void foo(int bar, std::string baz);
std::thread foo_thread(foo, 10, "ducks");
```

Care needs to be taken when passing pointer arguments since time
is needed convert a char\* into a std::string.  If this does not
finish before the parent exits the behavior is undefined.  Cast
before passing the argument to avoid dangling pointers.

References need to be forced because all parameters are by default
copies.

```cpp
void foo(int bar, std::string& baz);
std::string my_string = "Fish";
std::thread foo_thread(foo, 10, my_string);
```
Instead of being the reference to my_string, foo will receive a
reference to the std::thread internal copy of my_string.  Wrap
variables that are truly references in std::ref.

Similar to std::bind, member functions can be given as the 
function pointer, provided a valid object pointer is given.

std::move can be used to transfer unique_ptr ownership
into TLS.

### Thread Ownership
Ownership of threads can change:
* Function creates a background thread but hands back
  ownership
* Create a thread and then pass off ownership to a
  different function to wait on completion

Threads have std::move support.  Remember moving from temporary
objects is automatic and implicit. 

```cpp
std::thread thread1;
...
thread1=std::thread(foo); // move from temporary -> named where temporary
                          // is the return of std::thread
```
If an assigned thread object is reassigned a new function
it std::terminate() will be called.

**Standard Library**: std::mem_fn creates a call wrapper of unspecified type
                      given a pointer to a member that will be wrapped.  Stands
                      for "member function".

**Standard Library**: std::bind allows for a function to be created from another
                      g(x,y) = f(a, y, b, x) where a and b are already defined.

### Thread count at runtime
True concurrency limit can be determined from std::thread::hardware_concurrency();
however, 0 can be returned if the information is not available.


**Standard Library**: std::advance increments an interator by n.

### Thread Identification
The type of the identifier is std::thread::id and is accessible by the get_id() 
member function.  This type has a total ordering and allows for all comparisons.
