#include "Logger.hpp"

#include <thread>
#include <chrono>

int main() {
	Logger::init_logging("log.txt");
	Logger::log_info("this is info", 4, "value ", 4.555f);
	Logger::log_warn("this is warn");
	Logger::log_error("this is error");
	std::this_thread::sleep_for(std::chrono::seconds(5));
	return 0;
}