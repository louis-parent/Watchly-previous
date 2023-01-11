#ifndef __TASK_H__
#define __TASK_H__

#include <string>

namespace watchly
{
	struct Task
	{
		std::string date = "";
		double hours = 0.0;
		std::string label = "";
	};
}

#endif
