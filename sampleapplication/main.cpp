#include "Logger.hpp"


int main() {
	Logger::log_info("this is info");
	Logger::log_warn("this is warn");
	Logger::log_error("this is error");
	return 0;
}