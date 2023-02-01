#ifndef __TIME_UTILS_H__
#define __TIME_UTILS_H__

#include <string>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace watchly::utils
{
	class TimeUtils
	{
		public:
			static std::string format(const std::chrono::system_clock::time_point& point)
			{
				std::time_t timet = std::chrono::system_clock::to_time_t(point);
				std::tm time = *std::localtime(&timet);

				std::stringstream stream;
				stream << std::put_time(&time, "%d/%m/%Y");

				return stream.str();
			}
			
			static std::chrono::system_clock::time_point parse(const std::string& string)
			{
				std::tm time{};
				std::stringstream(string) >> std::get_time(&time, "%d/%m/%Y");
				return std::chrono::system_clock::from_time_t(std::mktime(&time));	
			}
	};
}

#endif
