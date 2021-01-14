/**
 * Small simulation where some cats get 
 * and eat cakes
 *
 * Explores the use of futures and promises to
 * synchronize events
 */

#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <vector>
#include <string>
#include <thread>
#include <iostream>

class Bakery {
public:
    Bakery():open_(true) { }
    ~Bakery() { }
    std::future<int> take_order();
    void close() { open_ = false; }
    void operate() { process_orders(); }
private:
    void process_orders();
    bool open_;
    std::mutex order_lock_;
    std::condition_variable order_cond_;
    std::queue<std::promise<int>> cake_orders_;
};


// Take an order from a cat
std::future<int> Bakery::take_order()
{
    std::promise<int> order_promise;
    std::future<int> order_future(order_promise.get_future());
    

    // push the order onto the bakery's order 
    // queue and wake up the baker if necessary
    std::lock_guard<std::mutex> lk(order_lock_);
    cake_orders_.push(std::move(order_promise));
    order_cond_.notify_one();
    return order_future;
}

// Process cake orders that have been sent by cats
void Bakery::process_orders()
{
    while (open_) {
        std::unique_lock<std::mutex> lk(order_lock_);
        if (cake_orders_.empty()) {
            /* no orders to process so go to sleep */
            order_cond_.wait(lk, [this]{ return open_; });
        }
        if (!cake_orders_.empty()) {
            std::promise<int> cake_order(std::move(cake_orders_.front()));
            // "Bake" the cake by setting a value
            cake_order.set_value(1);
            cake_orders_.pop();
        }
        lk.unlock();
        
    }
}

class Cat {
public:
    Cat(std::string name, int capacity):
        name_(name),
        capacity_(capacity) { }
    ~Cat() { }
    void be_a_cat(Bakery &bakery);
private:
    std::future<int> place_order(Bakery &bakery);
    void eat_cake(std::future<int> &&cake_order);
    void nap();
    
    std::string name_;
    int capacity_;
};

// Lead the life of a cat
void Cat::be_a_cat(Bakery &bakery)
{
    while (capacity_ > 0 ) {
        std::cout << name_ << " is being a cat" << std::endl;

        std::future<int> cake_order(std::move(place_order(bakery)));    
        
        std::cout << name_ << " ordered a cake" << std::endl;
        eat_cake(std::move(cake_order));

        std::cout << name_ << " is napping" << std::endl;
        nap();
    }
    
    std::cout << name_ << " ate too many cakes and exploded!" << std::endl;
}

std::future<int> Cat::place_order(Bakery &bakery)
{
    return bakery.take_order();
}


void Cat::eat_cake(std::future<int> &&cake_order)
{
    capacity_ -= cake_order.get();
}

void Cat::nap()
{
    std::chrono::seconds nap_time(3);
    std::this_thread::sleep_for(nap_time);
}

int main()
{
    std::vector<Cat> cats{{"Fluffy", 3},
                          {"Choo-Choo", 2},
                          {"Chonko", 4}};

    Bakery cat_cake_bakery;
    
    std::vector<std::thread> cat_threads;
    for (auto &cat : cats) {
        cat_threads.push_back(std::thread(&Cat::be_a_cat, &cat, std::ref(cat_cake_bakery)));
    }
    
    std::thread bakery_thread(&Bakery::operate, &cat_cake_bakery);

    for (auto &thread : cat_threads) {
        thread.join();
    }

    cat_cake_bakery.close();

    bakery_thread.join();
}
