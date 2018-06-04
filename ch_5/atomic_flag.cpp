#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <algorithm>
#include <functional>

std::atomic_flag af = ATOMIC_FLAG_INIT;


void set_flag() 
{
    std::cout << af.test_and_set() << std::endl;        
}


int main()
{
    int i; 
    std::vector<std::thread> thread_vec;

    for (i = 0; i < 10; ++i) {
        thread_vec.push_back(std::thread(set_flag));
    } 
    

    std::for_each(thread_vec.begin(), thread_vec.end(),
                  std::mem_fn(&std::thread::join));


    af.clear();
    return 0;
}
