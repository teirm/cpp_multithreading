// Implementation details for the logger class
//
// 16-January-2021

#include "Logger.hpp"

#include <iomanip>
#include <iostream>

void Logger::operator()()
{
    std::unique_lock<std::mutex> lk(log_lock_);
    while (running_) {
        if (log_queue_.empty()) {
            // only start displaying log messages if there are 
            // more than flush_size_ log messages in the queue
            // otherwise let them accumulate.
            log_cond_.wait(lk, [this]{return log_queue_.size() > flush_size_ || !running_;});
        } 
        if (running_ == false) {
            break;
        }
        auto log_entry = log_queue_.front();
        log_queue_.pop();
        lk.unlock();
        print_entry(log_entry);
        lk.lock();
    }

    while (!log_queue_.empty()) {
        auto log_entry = log_queue_.front();
        log_queue_.pop();
        print_entry(log_entry);
    }
}

void Logger::push_entry(LogEntry entry)
{
    std::lock_guard<std::mutex> lk(log_lock_);
    if (running_ == false) {
        // drop any messages that are pushed after the logger has stopped running
        return;
    }
    
    log_queue_.push(entry);
    log_cond_.notify_one();
    return;
}

void Logger::stop_logger()
{
    std::lock_guard<std::mutex> lk(log_lock_);
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
