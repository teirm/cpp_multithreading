/**
 * Messing around with C++ concurrency with some basic examples
 */

#include <vector>
#include <thread>
#include <mutex>
#include <numeric>
#include <iostream>

class PartialSums {
public:
    void push(int v) {
        std::lock_guard<std::mutex> lock(sums_lock);
        sums.push_back(v);
    }
    
    int accumulate() {
        std::lock_guard<std::mutex> lock(sums_lock);
        return std::accumulate(sums.begin(), sums.end(), 0);
    }

private:
    std::vector<int> sums;
    std::mutex sums_lock;
};

PartialSums p_sums;

void do_sum(std::vector<int>::iterator begin,
            std::vector<int>::iterator end)
{
    int partial = std::accumulate(begin, end, 0);
    p_sums.push(partial);
}


int main()
{
    std::vector<int>            numbers(1000000000,2);
    std::vector<std::thread>    threads;
    unsigned int chunks = std::thread::hardware_concurrency();
    unsigned int chunk_size = 0;

    if (chunks == 0) {
        chunks = 5;
        chunk_size = numbers.size()/chunks;
    } else {
        chunk_size = numbers.size()/chunks;
    }
    
    auto begin = numbers.begin();
    auto end = numbers.end();
    for (int i = 0; i < chunks-1; i++) {
        end = std::next(begin, chunk_size);
        threads.push_back(std::thread(do_sum, begin, end));
        begin = end;
    }
    threads.push_back(std::thread(do_sum, begin, numbers.end()));
    
    std::cout << "Threads Running: " << threads.size() << std::endl;

    for (auto &t : threads) {
        t.join();
    }

    std::cout << "Sum: " << p_sums.accumulate() << std::endl;
    
    return 0;
}
