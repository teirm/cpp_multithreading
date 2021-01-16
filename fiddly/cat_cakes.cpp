/**
 * Small simulation where some cats get 
 * and eat cakes
 *
 * Explores the use of futures and promises to
 * synchronize events
 */

#include <iomanip>
#include <chrono>
#include <condition_variable>
#include <future>
#include <mutex>
#include <queue>
#include <vector>
#include <string>
#include <thread>
#include <iostream>

using namespace std::chrono_literals;

struct LogEntry {
    std::string name;
    std::string message;
    std::time_t time;
};

class Logger {
    public:
    Logger(int delay):
        delay_(delay),
        running_(true) {}
    ~Logger() { }
    void operator()();
    void push_entry(LogEntry entry);
    void stop_logger();
private:
    int delay_;
    bool running_;

    void print_entry(const LogEntry &entry);

    std::mutex log_lock_;
    std::condition_variable log_cond_;
    std::queue<LogEntry> log_queue_;
};

void Logger::operator()()
{
    while (running_) {
        std::unique_lock<std::mutex> lk(log_lock_);
        if (log_queue_.empty()) {
            log_cond_.wait_for(lk, delay_*1s, [this](){return !log_queue_.empty() || !running_;});
        } else {
            auto log_entry = log_queue_.front();
            log_queue_.pop();
            print_entry(log_entry);
        }
        lk.unlock();
    }

    // flush the log queue before terminating
    std::unique_lock<std::mutex> lk(log_lock_);
    while (!log_queue_.empty()) {
        auto log_entry = log_queue_.front();
        log_queue_.pop();
        print_entry(log_entry);
    }
}

void Logger::push_entry(LogEntry entry)
{
    if (running_ == false) {
        // drop any messages that are pushed after the logger has stopped running
        return;
    }
    
    std::unique_lock<std::mutex> lk(log_lock_);
    log_queue_.push(entry);
    log_cond_.notify_one();
    return;
}

void Logger::stop_logger()
{
    std::unique_lock<std::mutex> lk(log_lock_);
    running_ = false;
    log_cond_.notify_one();
    return;
}


// print a long entry
void Logger::print_entry(const LogEntry &entry) 
{
    std::cout << "Time:  "  << std::put_time(std::localtime(&entry.time), "%F %T") << std::endl 
              << "Name:  " << entry.name << std::endl 
              << "Event: " << entry.message << std::endl 
              << std::endl; 
}

class Bakery {
public:
    Bakery(Logger &logger):
        logger_(logger), 
        open_(true) { }
    ~Bakery() { }
    std::future<int> take_order();
    void close() { std::unique_lock<std::mutex> lk(order_lock_); open_ = false; order_cond_.notify_one(); }
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

class Cat {
public:
    Cat(std::string name, int capacity, int nap_time, Logger &logger):
        name_(name),
        capacity_(capacity),
        nap_time_(nap_time),
        logger_(logger){ }
    ~Cat() { }
    void operator()(Bakery &bakery);
private:
    std::future<int> place_order(Bakery &bakery);
    void eat_cake(std::future<int> &&cake_order);
    void nap();
    void log_entry(std::string message);

    Logger &logger_;
    std::string name_;
    int capacity_;
    int nap_time_;
};

// Log a message for a cat using its name and
// the even that occured
void Cat::log_entry(std::string message)
{
    LogEntry entry{name_, message, 0};
    
    entry.time = std::time(nullptr);
    if (entry.time == -1) {
        std::cerr << "Error retrieving time" << std::endl;
    }
    logger_.push_entry(entry);
}

// Lead the life of a cat
void Cat::operator()(Bakery &bakery)
{
    while (capacity_ > 0 ) {
        log_entry("is being a cat");
        std::future<int> cake_order(std::move(place_order(bakery)));    
        
        log_entry("ordered a cake");
        eat_cake(std::move(cake_order));

        log_entry("is napping");
        nap();
    }
    
    log_entry("ate too many cakes and exploded!");
}

// place an order at the bakery. the bakery returns
// a future and handles the promise to make the cake
std::future<int> Cat::place_order(Bakery &bakery)
{
    return bakery.take_order();
}

// cat eats a cake it received from the bakery
//
// It might need to wait for the cake before eating it
void Cat::eat_cake(std::future<int> &&cake_order)
{
    capacity_ -= cake_order.get();
}


// cats nap for some period of time and then wake up
void Cat::nap()
{
    std::chrono::seconds nap_time(nap_time_);
    std::this_thread::sleep_for(nap_time);
}

int main()
{
    constexpr int logger_delay = 5;
    Logger logger(logger_delay);
    
    // std::thread takes a reference to a callable
    // object
    std::thread logger_thread(std::ref(logger));
    
    // create the bakery
    Bakery cat_cake_bakery(logger);
    std::thread bakery_thread(std::ref(cat_cake_bakery));
    
    // create all the cats
    std::vector<Cat> cats{{"Fluffy", 3, 4, logger},
                          {"Choo-Choo", 2, 3, logger},
                          {"Chonko", 4, 5, logger}};

    
    // create a vector of cat threads
    std::vector<std::thread> cat_threads;
    for (auto &cat : cats) {
        // reference arguments must be passed by std::ref
        // can't pass a non-const rvalue 
        cat_threads.push_back(std::thread(cat, std::ref(cat_cake_bakery)));
    }
    
    // join cat threads as the cats explode
    for (auto &cat_thread : cat_threads) {
        cat_thread.join();
    }
    
    // close the bakery
    cat_cake_bakery.close();
    bakery_thread.join();

    // stop logging
    logger.stop_logger();
    logger_thread.join();
}
