/*
 * Using a vector in a thread safe manner
 */

#include <mutex>
#include <vector>
#include <functional>
#include <thread>
#include <stdexcept>
#include <string>
#include <iostream>

template <class T> 
class ThreadSafeVector {
public:
    void add(T item) {
        std::lock_guard<std::mutex> gl(lock);
        this->container.push_back(item);
    }

    void get() {
        std::lock_guard<std::mutex> gl(lock);
        if (this->container.size() == 0) {
            throw std::range_error("Size = 0");
        }
        this->container.pop_back();
    }

    void display() {
        std::lock_guard<std::mutex> gl(lock);
        for (T& value : container) {
            std::cout << value << std::endl;
        }
    }

private:
    std::mutex               lock;
    std::vector<T>           container;
};


void producer(ThreadSafeVector<std::string>& ts_vector)
{
    for (int i = 0; i < 20; i++) { 
        ts_vector.add("blep");
    }
}


void consumer(ThreadSafeVector<std::string>& ts_vector)
{
    for (int i = 0; i < 20; i++) {
        try {
            ts_vector.get();
        } catch(const std::range_error& re) {
            std::cout << re.what() << std::endl;
        }
    }
}

int main()
{
    ThreadSafeVector<std::string> string_vector;

    std::thread f(producer, std::ref(string_vector));
    std::thread k(consumer, std::ref(string_vector));

    f.join();
    k.join();

    string_vector.display();
    

    return 0;
}
