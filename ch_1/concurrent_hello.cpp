/*
 * Concurrent Hello World program
 * Date: 01 March 2018
 */
#include <iostream>
#include <thread>       // provides c++11 multithreading support

void hello()
{
    std::cout << "Hello concurrency\n";
}

int main()
{
    std::thread t(hello);   // create a thread object
    t.join();           // wait until t finishes
}
