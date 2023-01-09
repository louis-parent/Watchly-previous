#include "Core.hpp"

#include <exception>
#include <cmath>

using namespace watchly;

const std::string Core::WATCHLY_DIRECTORY = std::string(std::getenv("HOME")) + "/.watchly";
const std::string Core::TIME_TABLE_FILE = "/timetable.csv";
const std::string Core::MARKER_FILE = "/marker.time";
			
void Core::start() const
{
	std::ofstream markerFile = this->openWatchlyFile<std::ofstream>(Core::MARKER_FILE, std::ios_base::out);
	
	if(markerFile)
	{
		const auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
		
		markerFile << now.time_since_epoch().count();
		markerFile.close();
	}
	else
	{
		throw std::ios_base::failure("Cannot create marker file");
	}
}

double Core::stop(const std::string& label) const
{
	const auto marker = this->getCurrentMarker();
	
	if(marker)
	{
		const auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
		double duration = std::chrono::duration<double, std::chrono::hours::period>(now - marker.value()).count();
		
		// Floor duration to 5min
		duration = duration - std::fmod(duration, (5.0 / 60.0));

		this->insertEntry(duration, label);
		
		try
		{
			this->removeWatchlyFile(Core::MARKER_FILE);
		}
		catch(const std::exception& e)
		{
			throw std::runtime_error(std::string("Task saved but : ") + e.what());
		}
		
		return duration;
	}
	else
	{
		throw std::logic_error("Cannot stop a not running chronometer");
	}
}

std::vector<Task> Core::getHistory() const
{
	std::ifstream timeTable = this->openWatchlyFile<std::ifstream>(Core::TIME_TABLE_FILE, std::ios_base::in);
	
	if(timeTable)
	{
		std::vector<Task> tasks;
		
		std::string line;
		while(std::getline(timeTable, line))
		{
			Task task;

			std::stringstream row(line);
			std::string value;
			
			std::getline(row, value, ';');
			task.date = value;

			std::getline(row, value, ';');
			task.hours = value;
			
			std::getline(row, value, ';');
			task.label = value;
			
			tasks.push_back(task);
		}
		
		return tasks;
	}
	else
	{
		throw std::runtime_error("Cannot find time table");
	}
}

void Core::cancel() const
{
	try
	{
		this->removeWatchlyFile(Core::MARKER_FILE);
	}
	catch(const std::exception& e)
	{
		throw std::logic_error("Cannot cancel a not running chronometer");
	}
}

std::optional<std::chrono::time_point<std::chrono::system_clock>> Core::getCurrentMarker() const
{
	std::ifstream markerFile = this->openWatchlyFile<std::ifstream>(Core::MARKER_FILE, std::ios_base::in);
	
	if(markerFile)
	{
		long millis = 0;
		markerFile >> millis;
		markerFile.close();
		
		return std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(millis));
	}
	else
	{
		return std::nullopt;
	}
}

void Core::insertEntry(double duration, const std::string& label) const
{
	std::ofstream timeTable = this->openWatchlyFile<std::ofstream>(Core::TIME_TABLE_FILE, std::ios_base::out | std::ios_base::app);
	
	if(timeTable)
	{
		auto time = std::time(nullptr);
		auto local = *std::localtime(&time);
		
		timeTable << std::put_time(&local, "%d/%m/%Y") << ";"<< duration << ";" << label << std::endl;	
	}
	else
	{
		throw std::ios_base::failure("Cannot write into time table file");
	}
}

void Core::removeWatchlyFile(const std::string& file) const
{
	bool success = false;
	
	try
	{
		success = std::filesystem::remove(Core::WATCHLY_DIRECTORY + file);
		
	}
	catch(const std::exception& e)
	{
		throw std::ios_base::failure(std::string("Cannot enter directory ") + Core::WATCHLY_DIRECTORY);
	}

	if(!success)
	{
		throw std::ios_base::failure(std::string("Cannot remove ") + file + " file");
	}
}
