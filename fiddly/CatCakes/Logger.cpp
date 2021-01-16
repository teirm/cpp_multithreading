// Implementation details for the logger class
//
// 16-January-2021

#include "Logger.hpp"

#include <iomanip>
#include <iostream>

using namespace std::chrono_literals;

void Logger::operator()()
{
    while (running_) {
        std::unique_lock<std::mutex> lk(log_lock_);
        if (log_queue_.empty()) {
            log_cond_.wait_for(lk, delay_*1s, [this]{return !log_queue_.empty() || !running_;});
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
