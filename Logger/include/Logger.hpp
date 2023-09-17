#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

static std::string getCurrentTimeWithMilliseconds() {
    using namespace std::chrono;

    // Capture the current time
    system_clock::time_point now = system_clock::now();

    // Separate into time and milliseconds
    time_t now_tt = system_clock::to_time_t(now);
    auto duration_since_epoch = now.time_since_epoch();
    auto seconds_since_epoch = duration_cast<seconds>(duration_since_epoch);
    auto millis = duration_cast<milliseconds>(duration_since_epoch - seconds_since_epoch);

    // Convert time to string
    struct tm timeinfo;
    char buffer[80];
    localtime_s(&timeinfo, &now_tt);
    strftime(buffer, sizeof(buffer), "%H:%M:%S", &timeinfo);

    // Add milliseconds with zero-padding
    std::ostringstream oss;
    oss << buffer << "." << std::setw(3) << std::setfill('0') << millis.count();

    return oss.str();
}
namespace Logger {
    enum class LogLevel {
        info,
        error,
        warning,
        trace
    };

    struct LogMessage {
        LogLevel level;
        std::string message;
    };

    static bool initialized = false;
    static bool is_running = true;
    static std::mutex log_mutex;
    static std::mutex queue_mutex;
    static std::condition_variable condition_var;
    static std::queue<LogMessage> log_queue;
    static std::ofstream log_file;

    inline void write_log(const LogMessage& log_msg) {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        std::string timestamp = getCurrentTimeWithMilliseconds();

        std::string level_str;
        switch (log_msg.level) {
        case LogLevel::info:
            level_str = "info";
            break;
        case LogLevel::error:
            level_str = "error";
            break;
        case LogLevel::warning:
            level_str = "warning";
            break;
        default:
            level_str = "trace";
            break;
        }

        std::string log_entry = "[" + timestamp + "][" + level_str + "] " + log_msg.message + "\n";

        std::cout << log_entry;  // Console log
        if (log_file.is_open()) {
            log_file << log_entry;  // File log
        }
    }

    inline void logging_thread() {
        while (is_running) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            condition_var.wait(lock, [] { return !log_queue.empty() || !is_running; });
            while (!log_queue.empty()) {
                LogMessage log_msg = log_queue.front();
                log_queue.pop();
                write_log(log_msg);
            }
            lock.unlock();
        }
    }

    inline void init_logging(const std::string& file) {
        log_file.open(file, std::ios::out | std::ios::app);
        initialized = true;
        std::thread log_thread(logging_thread);
        log_thread.detach();
    }

    inline void init_console_logging() {
        initialized = true;
        std::thread log_thread(logging_thread);
        log_thread.detach();
    }

    inline void shutdown() {
        is_running = false;
        condition_var.notify_one();
    }

    template<typename... Args>
    inline void enqueue_message(LogLevel level, Args... args) {
        std::ostringstream stream;
        ((stream << args << " "), ...);
        LogMessage log_msg{ level, stream.str() };

        std::lock_guard<std::mutex> lock(queue_mutex);
        log_queue.push(log_msg);
        condition_var.notify_one();
    }

    template<typename... Args>
    inline void log_info(Args... args) {
        if (!initialized) {
            init_console_logging();
        }
        enqueue_message(LogLevel::info, args...);
    }

    template<typename... Args>
    inline void log_error(Args... args) {
        if (!initialized) {
            init_console_logging();
        }
        enqueue_message(LogLevel::error, args...);
    }

    template<typename... Args>
    inline void log_warn(Args... args) {
        if (!initialized) {
            init_console_logging();
        }
        enqueue_message(LogLevel::warning, args...);
    }
}
