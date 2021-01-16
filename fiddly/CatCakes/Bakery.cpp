// Bakery.H
//
// Implementation details of Bakery class
//
// Mostly an exploation of C++ concurrency features
// 16-January-2021

#include <iostream>
#include <sstream>

#include "Bakery.hpp"
#include "Logger.hpp"

// Close the bakery
void Bakery::close()
{
    std::unique_lock<std::mutex> lk(order_lock_);
    open_ = false;
    order_cond_.notify_one();
}

// Log an entry for the bakery
void Bakery::log_entry(std::string message)
{
    LogEntry entry{"bakery", message, 0};

    entry.time = std::time(nullptr);
    if (entry.time == -1) {
        std::cerr << "Error retrieving bakery time" << std::endl;
    }
    logger_.push_entry(entry);
}


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
    int total_cakes = 0;

    while (open_) {
        std::unique_lock<std::mutex> lk(order_lock_);
        if (cake_orders_.empty()) {
            // no orders to process so go to sleep
            order_cond_.wait(lk, [this]{ return !cake_orders_.empty() || !open_; });
        } else {
            std::promise<int> cake_order(std::move(cake_orders_.front()));
            // "Bake" the cake by setting a value
            log_entry("baked a cake");
            cake_order.set_value(1);
            total_cakes++;
            cake_orders_.pop();
        }
        lk.unlock();
    }

    std::ostringstream oss;
    oss << "Total Cakes Baked: " << total_cakes;
    log_entry(oss.str());
}
