# Chapter 2

## Terms

## Includes

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
