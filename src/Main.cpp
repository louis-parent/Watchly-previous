#include <iostream>
#include <iomanip>
#include <string>
#include "watchly/Core.hpp"

using namespace watchly;

void usage(const std::string& name)
{
	std::cerr << "❓ Usage : " << name << " [command] [option]" << std::endl;
	std::cerr << "Command can be : start, stop" << std::endl;
}

int main(int argc, const char* argv[])
{
	try
	{
		if(argc >= 2)
		{
			const Core core;
			const std::string command(argv[1]);
			
			if(command == "start" && argc == 2)
			{
				core.start();
				std::cout << "⌚ Chronometer started !" << std::endl;
			}
			else if(command == "stop" && argc == 3)
			{
				double hours = core.stop(argv[2]);
				std::cout << "📝 Chronometer stopped and task added to time table with " << hours << "h" << std::endl;
			}
			else if(command == "history" && argc == 2)
			{
				std::cout << std::fixed << std::setprecision(2);
				
				for(Task task : core.getHistory())
				{
					std::cout << "[" << task.date << "] " << task.label << " (" << task.hours << "h)" << std::endl; 
				}
			}
			else if(command == "cancel" && argc == 2)
			{
				core.cancel();
				std::cout << "🛑 Chronometer canceled..." << std::endl;
			}
			else
			{
				usage(argv[0]);
			}
		}
		else
		{
			usage(argv[0]);
		}

		return 0;
	}
	catch(const std::exception& e)
	{
		std::cout << "❌ " << e.what() << std::endl;
		return 1;
	}
	
}
