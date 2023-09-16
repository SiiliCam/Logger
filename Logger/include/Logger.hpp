#pragma once

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/support/date_time.hpp>

namespace Logger {
	static bool initialized = false;

	inline void init_logging(const std::string& file) {
		// Set up file sink
		auto file_sink = boost::log::add_file_log(file);
		file_sink->locked_backend()->auto_flush(true);  // Enable auto-flushing after each log record

		// Set up console sink
		auto console_sink = boost::log::add_console_log(std::clog);
		console_sink->locked_backend()->auto_flush(true);  // Enable auto-flushing after each log record

		console_sink->set_formatter(
			boost::log::expressions::stream
			<< "[" << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f") << "]"
			<< "[" << boost::log::trivial::severity << "]: "
			<< boost::log::expressions::message
		);

		// Set severity filter
		boost::log::core::get()->set_filter(
			boost::log::trivial::severity >= boost::log::trivial::info
		);

		// Add common attributes like timestamp, thread id, etc.
		boost::log::add_common_attributes();
		initialized = true;
	}
	inline void init_console_logging() {
		// Set up console sink
		auto console_sink = boost::log::add_console_log(std::clog);
		console_sink->locked_backend()->auto_flush(true);  // Enable auto-flushing after each log record

		console_sink->set_formatter(
			boost::log::expressions::stream
			<< "[" << boost::log::expressions::format_date_time<boost::posix_time::ptime>("TimeStamp", "%H:%M:%S.%f") << "]"
			<< "[" << boost::log::trivial::severity << "]: "
			<< boost::log::expressions::message
		);

		// Set severity filter
		boost::log::core::get()->set_filter(
			boost::log::trivial::severity >= boost::log::trivial::info
		);

		// Add common attributes like timestamp, thread id, etc.
		boost::log::add_common_attributes();

		initialized = true;
	}

	inline void init_if_needed() {
		if (!initialized) {
			init_console_logging();
		}
	}

	template<typename First, typename... Rest>
	inline void log_message(boost::log::trivial::severity_level level, First first, Rest... rest) {
		std::ostringstream stream;
		stream << first;
		((stream << " " << rest), ...);

		switch (level) {
		case boost::log::trivial::info:
			BOOST_LOG_TRIVIAL(info) << stream.str();
			break;
		case boost::log::trivial::error:
			BOOST_LOG_TRIVIAL(error) << stream.str();
			break;
		case boost::log::trivial::warning:
			BOOST_LOG_TRIVIAL(warning) << stream.str();
			break;
			// Add other severity levels as needed
		default:
			BOOST_LOG_TRIVIAL(trace) << stream.str();
			break;
		}
	}

	template<typename... Args>
	inline void log_info(Args... args) {
		init_if_needed();
		log_message(boost::log::trivial::info, args...);

	}

	template<typename... Args>
	inline void  log_error(Args... args) {
		init_if_needed();

		log_message(boost::log::trivial::error, args...);

	}

	template<typename... Args>
	inline void log_warn(Args... args) {
		init_if_needed();

		log_message(boost::log::trivial::warning, args...);

	}
}  // namespace Logger