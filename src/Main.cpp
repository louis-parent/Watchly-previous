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
			else if(command == "pause" && argc == 2)
			{
				core.pause();
				std::cout << "⌛ Chronometer paused..." << std::endl;
			}
			else if(command == "cancel" && argc == 2)
			{
				core.cancel();
				std::cout << "🛑 Chronometer canceled..." << std::endl;
			}
			else if(command == "status" && argc == 2)
			{
				const std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> sinceMarker = core.getTimeSinceMarker();	
				if(sinceMarker)
				{
					std::cout << "⏳ Chronometer started since " << std::chrono::duration<double, std::chrono::hours::period>(sinceMarker.value()).count() << "h !" << std::endl;
				}
				else
				{
					std::cout << "⌛ No chronometer are currently running" << std::endl;
				}

				const std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> bufferTime = core.getBufferedTime();
				if(bufferTime)
				{
					std::cout << "📬 Paused chronometer contains " << std::chrono::duration<double, std::chrono::hours::period>(bufferTime.value()).count() << "h to be validated..." << std::endl;
				}
				else
				{
					std::cout << "📭 Paused chronometer is currently empty" << std::endl;
				}
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
