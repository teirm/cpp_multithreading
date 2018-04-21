/* Listing 3.1 */

#include <list>
#include <mutex>
#include <algorithm>
#include <thread>
#include <vector>
#include <functional>
#include <iostream>

std::list<int> some_list;
std::mutex some_mutex;

void 
add_to_list(
    int         new_value) 
{
    std::lock_guard<std::mutex> guard(some_mutex);
    some_list.push_back(new_value);
}

int main()
{
    std::vector<std::thread> thread_vec;

    for (int i = 0; i < 20; ++i) {
        thread_vec.push_back(std::thread(add_to_list, i));
    }

    std::for_each(thread_vec.begin(), thread_vec.end(), 
            std::mem_fn(&std::thread::join));

    for (int i : some_list) {
        std::cout << i << std::endl;
    }

    return 0;
}
