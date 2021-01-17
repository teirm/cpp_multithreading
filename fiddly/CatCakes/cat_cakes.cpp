/**
 * Small simulation where some cats get 
 * and eat cakes
 *
 * Explores the use of futures and promises to
 * synchronize events
 */

#include <vector>
#include <thread>

#include "Cat.hpp"
#include "Bakery.hpp"
#include "Logger.hpp"

int main()
{
    constexpr size_t flush_size = 5; 
    Logger logger(flush_size);
    
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
