#include <iostream>
#include <iomanip>
#include <string>
#include "watchly/Core.hpp"

using namespace watchly;

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
				std::cout << "💾 Chronometer stopped and task added to time table with " << std::fixed << std::showpoint << std::setprecision(2) << hours << "h" << std::endl;
			}
			else if(command == "history" && argc == 2)
			{
				std::cout << "📑 List of tasks saved : " << std::endl;
				
				for(Task task : core.getHistory())
				{
					std::cout << "[" << task.date << "] " << task.label << " (" << std::fixed << std::showpoint << std::setprecision(2) << task.hours << "h)" << std::endl; 
				}
			}
			else if(command == "pause" && argc == 2)
			{
				core.pause();
				std::cout << "⏸ Chronometer paused..." << std::endl;
			}
			else if(command == "cancel" && argc == 2)
			{
				core.cancel();
				std::cout << "⏹ Chronometer canceled !" << std::endl;
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
			else if(command == "help" && argc == 2)
			{
				std::cout << "❓ Usage : " << argv[0] << " [command] [option]..." << std::endl;
				std::cout << "Available commands :" << std::endl;
				std::cout << "  help               \tDisplay the current documentation." << std::endl;
				std::cout << "  start              \tStart the chronometer." << std::endl;
				std::cout << "  stop [label]       \tStop the running chronometer and save the task. An option is required for the task name." << std::endl;
				std::cout << "  pause              \tPause the running chronometer and buffer it." << std::endl;
				std::cout << "  cancel             \tCancel running and paused chronometers." << std::endl;
				std::cout << "  history            \tDisplay a list of the saved tasks." << std::endl;
				std::cout << "  status             \tDisplay the current status of the running and paused chronometers." << std::endl;
				std::cout << "  put [hours] [label]\tDirectly add a task into the time table. Two options are required, one for the hours and a second for the label." << std::endl;
				
			}
			else if(command == "put" && argc == 4)
			{
				double hours = std::stod(argv[2]);
				const char* label = argv[3];
				
				core.putTask(hours, label);
				
				std::cout << "💾 The task '" << label << "' with " << std::fixed << std::showpoint << std::setprecision(2) << hours << "h was added to time table." << std::endl;
			}
			else
			{
				throw std::runtime_error(std::string("Invalid command, run `") + argv[0] + " help` to learn more");
			}
		}
		else
		{
			throw std::runtime_error(std::string("A command is required, run `") + argv[0] + " help` to learn more");
		}

		return 0;
	}
	catch(const std::exception& e)
	{
		std::cout << "❌ " << e.what() << std::endl;
		return 1;
	}
	
}
