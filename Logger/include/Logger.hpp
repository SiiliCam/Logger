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
#include <functional>
#include <iomanip>

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

#ifdef _WIN32 // Windows-specific
    localtime_s(&timeinfo, &now_tt);
#else // Unix-like systems
    localtime_r(&now_tt, &timeinfo);
#endif

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
    using LogFunction = std::function<void(const std::string&)>;

    class Logger {
    private:
        bool initialized = false;
        std::atomic_bool is_running;
        std::mutex log_mutex;
        std::mutex queue_mutex;
        std::thread m_loggingThread;
        std::condition_variable condition_var;
        std::queue<LogMessage> log_queue;
        std::ofstream log_file;
        LogFunction log_function;


        template<typename... Args>
        void enqueue_message(LogLevel level, Args... args) {
            std::ostringstream stream;
            ((stream << args << " "), ...);
            LogMessage log_msg{ level, stream.str() };

            std::lock_guard<std::mutex> lock(queue_mutex);
            log_queue.push(log_msg);
            condition_var.notify_one();
        }

    public:
        Logger() : is_running(true), log_function([](const std::string& msg) { std::cout << msg << std::endl; }) {}
        ~Logger() {
            is_running = false;
            condition_var.notify_one();
            std::cout << "joining logging thread" << std::endl;
            if (m_loggingThread.joinable()) {
                m_loggingThread.join();
            }
            std::cout << "logging thread joined" << std::endl;
        }
        static Logger& getInstance() {
            static Logger instance;
            return instance;
        }
        void setLogFunction(LogFunction func) {
            log_function = func;
        }
        void init_logging(const std::string& file) {
            log_file.open(file, std::ios::out | std::ios::app);
            initialized = true;
            m_loggingThread = std::thread(&Logger::logging_thread, this);
        }
        void write_log(const LogMessage& log_msg) {
            auto now = std::chrono::system_clock::now();
            auto time_t_now = std::chrono::system_clock::to_time_t(now);
            std::string timestamp = getCurrentTimeWithMilliseconds();

            std::string level_str;
            std::string color_code;

            switch (log_msg.level) {
            case LogLevel::info:
                level_str = "info";
                color_code = "\033[32m";  // ANSI code for green
                break;
            case LogLevel::error:
                level_str = "error";
                color_code = "\033[31m";  // ANSI code for red
                break;
            case LogLevel::warning:
                level_str = "warning";
                color_code = "\033[33m";  // ANSI code for yellow
                break;
            default:
                level_str = "trace";
                color_code = "\033[0m";  // ANSI code for default
                break;
            }

            std::string log_entry = "[" + timestamp + "][" + level_str + "]\t" + log_msg.message + "\n";

            // Bold for timestamp is \033[1m and \033[0m to reset
            std::string colored_log_entry = "\033[1m[" + timestamp + "]\033[0m[" + color_code + level_str + "\033[0m]\t" + log_msg.message;
            log_function(colored_log_entry);

            if (log_file.is_open()) {
                log_file << log_entry;  // File log (uncolored)
                log_file.flush();
            }
            else {
                std::cout << "log file is not open" << std::endl;
            }
        }
        void logging_thread() {
            while (is_running.load()) {
                std::unique_lock<std::mutex> lock(queue_mutex);
                condition_var.wait(lock, [this] { return !log_queue.empty() || !is_running.load(); });
                while (!log_queue.empty() && is_running.load()) {
                    try {

                        LogMessage log_msg = log_queue.front();
                        log_queue.pop();
                        write_log(log_msg);
                    }
                    catch (const std::exception& e) {
                        std::cout << "logger error " << e.what()  << std::endl;
                        break;
                    }
                }
                lock.unlock();
            }
        }

        template<typename... Args>
        void log_info(Args... args) {
            if (!initialized) {
                init_logging("default.log");
            }
            enqueue_message(LogLevel::info, args...);
        }

        template<typename... Args>
        void log_error(Args... args) {
            if (!initialized) {
                init_logging("default.log");
            }
            enqueue_message(LogLevel::error, args...);
        }
        template<typename... Args>
        inline void log_warn(Args... args) {
            if (!initialized) {
                init_logging("default.log");
            }
            enqueue_message(LogLevel::warning, args...);
        }
        // Other log levels here...
    };

    template<typename... Args>
    inline void init_logging(const std::string& file, Args... args) {
        Logger::getInstance().init_logging(file);
    }

    template<typename... Args>
    inline void log_info(Args... args) {
        Logger::getInstance().log_info(args...);
    }

    template<typename... Args>
    inline void log_error(Args... args) {
        Logger::getInstance().log_error(args...);
    }
    template<typename... Args>
    inline void log_warn(Args... args) {
        Logger::getInstance().log_warn(args...);
    }
    // Add other logging methods as needed
}
