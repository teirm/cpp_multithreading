/* Examples and ideas 
 * from chapter 2 of C++ multithreading
 * Date 6 April 2018 
 */

#include <thread>
#include <iostream>
#include <functional>
#include <string>


struct Foo {
    void hello() {
        std::cout << "Hello" << std::endl;
    }
};

void print_hello() {
    std::cout << "Hello" << std::endl;
}

int main()
{
    std::thread my_thread([] () {
            print_hello();
            });
    my_thread.join();

    Foo f;
    auto hi = std::mem_fn(&Foo::hello);
    hi(f);

    return 0;
}
