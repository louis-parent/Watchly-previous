#ifndef __TASK_H__
#define __TASK_H__

#include <string>

namespace watchly
{
	struct Task
	{
		std::string date = "";
		std::string hours = "";
		std::string label = "";
	};
}

#endif
