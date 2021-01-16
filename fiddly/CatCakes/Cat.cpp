// Implementation of the Cat class
//
// 16-January-2021

#include "Cat.hpp"
#include "Bakery.hpp"
#include "Logger.hpp"

#include <iostream>

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
