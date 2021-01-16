// The all important Cat class. This class
// represents a cat that orders some cakes,
// eats some cakes, and naps. 
//
// Cats have a finite capacity -- so if they 
// eat too many cakes they explode and can
// no longer order cakes.
//

#pragma once

#include <string>
#include <future>

class Logger;
class Bakery;

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

    std::string name_;
    int capacity_;
    int nap_time_;
    Logger &logger_;
};
