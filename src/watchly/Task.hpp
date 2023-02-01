#ifndef __TASK_H__
#define __TASK_H__

#include <string>
#include <chrono>

namespace watchly
{
	struct Task
	{
		std::chrono::system_clock::time_point date{};
		double hours = 0.0;
		std::string label{};
	};
}

#endif
