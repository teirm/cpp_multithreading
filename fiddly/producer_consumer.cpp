/**
 * Very simple producer consumer to use condition
 * variables in C++
 */
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>


bool done = false;
std::queue<int> input_values;

std::mutex queue_lock;
std::condition_variable processed_cond;
std::queue<int> processed_values;

void processing_thread()
{
    int value = 0;
    while(input_values.empty() == false) {
        value = input_values.front();
        input_values.pop();
        {
            std::lock_guard<std::mutex> lk(queue_lock);
            processed_values.push(value*3);
        }
        processed_cond.notify_one();
    }
    done = true;
    processed_cond.notify_one();
}

void consuming_thread()
{
    int sum = 0;
    while (done == false || !processed_values.empty()) {
        std::unique_lock<std::mutex> lk(queue_lock);
        processed_cond.wait(lk, []{return (!processed_values.empty() || done); });
        if (!processed_values.empty()) {
            sum += processed_values.front();
        }
        processed_values.pop();
        lk.unlock();
    }
    std::cout << "Final Sum: " << sum << std::endl;
}

int main()
{
    /* initialize the input values */
    for (int i = 0; i < 100; i++) {
        input_values.push(1);
    }

    std::thread processor(processing_thread);
    std::thread consumer(consuming_thread);

    processor.join();
    consumer.join();

    return 0;
}
