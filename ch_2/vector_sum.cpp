/* Thread Library Practice
 * Date:     15 April 2018
 */

#include <thread>
#include <iostream>
#include <vector>
#include <algorithm>
#include <functional>

void walk_left(
    int&        i) {
    i--;
}

void walk_right(
    int&        i) {
    i+=2;
}

void get_id(std::thread t) {
    std::cout << t.get_id() << std::endl;

    t.join();
}

int main()
{
    std::vector<std::thread> threads;
    
    int pos = 0;

    for (unsigned int i = 0; i < 20; ++i) {
        if (i % 2 == 0) {
            threads.push_back(std::thread(walk_left, std::ref(pos)));
        } else {
            threads.push_back(std::thread(walk_right, std::ref(pos)));
        }
    }
    std::for_each(threads.begin(), threads.end(),
                  std::mem_fn(&std::thread::join));

    std::cout << pos << std::endl;

    std::thread f(walk_left, std::ref(pos));
    get_id(std::move(f));

    return 0;
}
