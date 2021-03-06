// Logger class to handle all event logging
// in the multi threaded cat cake environment.
// This avoids message mangling and the need
// to protect the output buffer in multiple
// threads.
//
// This class periodically flushes any messages
// it finds and then waits for new messages
// to be added.
//
// 16-January-2021

#pragma once

#include <string>
#include <chrono>
#include <mutex>
#include <condition_variable>
#include <queue>

struct LogEntry {
    std::string name;
    std::string message;
    std::time_t time;
};

class Logger {
    public:
    Logger(int flush_size):
        running_(true),
        flush_size_(flush_size) {}
    ~Logger() { }
    void operator()();
    void push_entry(LogEntry entry);
    void stop_logger();
private:
    bool    running_;
    size_t  flush_size_;
    std::mutex log_lock_;
    std::condition_variable log_cond_;
    std::queue<LogEntry> log_queue_;
    
    void print_entry(const LogEntry &entry);
};
