// Bakery.H
//
// Class representing a bakery from which cakes can be
// concurrently ordered in the form of futures
//
// Mostly an exploation of C++ concurrency features
// 16-January-2021

#pragma once

#include <future>
#include <mutex>
#include <condition_variable>
#include <queue>

// Forward declare the logger class
class Logger;

class Bakery {
public:
    Bakery(Logger &logger):
        logger_(logger), 
        open_(true) { }
    ~Bakery() { }
    std::future<int> take_order();
    void close(); 
    void operator()() { process_orders(); }
private:
    void process_orders();
    void log_entry(std::string message);
    Logger &logger_;
    bool open_;
    std::mutex order_lock_;
    std::condition_variable order_cond_;
    std::queue<std::promise<int>> cake_orders_;
};
