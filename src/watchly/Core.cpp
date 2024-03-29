#include "Core.hpp"

#include <exception>
#include <cmath>
#include <ranges>
#include "utils/TimeUtils.hpp"
#include "utils/InvoiceUtils.hpp"
#include "templating/Latex.hpp"
#include "utils/MapUtils.hpp"

using namespace watchly;

const std::string Core::WATCHLY_DIRECTORY = std::string(std::getenv("HOME")) + "/.watchly";
const std::string Core::TIME_TABLE_FILE = Core::WATCHLY_DIRECTORY + "/timetable.csv";
const std::string Core::MARKER_FILE = Core::WATCHLY_DIRECTORY + "/marker.time";
const std::string Core::BUFFER_FILE = Core::WATCHLY_DIRECTORY + "/buffer.time";
const std::string Core::TEMPLATE_FILE = Core::WATCHLY_DIRECTORY + "/template.tex";
const std::string Core::PROPERTIES_FILE = Core::WATCHLY_DIRECTORY + "/global.properties";

void Core::start() const
{
	if(!this->isRunning())
	{
		bool success = this->write(
			Core::MARKER_FILE,
			std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now()).time_since_epoch().count()
		);
		
		if(!success)
		{
			throw std::runtime_error("Failed to start chronometer");
		}
	}
	else
	{
		throw std::logic_error("Cannot start a chronometer, one is already running");
	}
}

double Core::stop(const std::string& label) const
{
	const std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> bufferTime = this->getBufferedTime();
	const std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> sinceMarker = this->getTimeSinceMarker();
	
	if(sinceMarker || bufferTime)
	{
		std::chrono::duration<long, std::chrono::milliseconds::period> duration = std::chrono::duration<long, std::chrono::milliseconds::period>::zero();
		
		if(bufferTime)
		{
			duration += bufferTime.value();
			this->resetBuffer();
		}
		
		if(sinceMarker)
		{
			duration += sinceMarker.value();
			this->stopRunning();
		}
		
		double hours = std::chrono::duration<double, std::chrono::hours::period>(duration).count();
		
		// Floor duration to 5min
		hours = hours - std::fmod(hours, (5.0 / 60.0));

		this->putTask(hours, label);
		
		return hours;
	}
	else
	{
		throw std::logic_error("Cannot stop a not running or buffered chronometer");
	}
}

std::vector<watchly::model::Task> Core::getHistory() const
{
	std::ifstream timeTable = this->openWatchlyFile<std::ifstream>(Core::TIME_TABLE_FILE, std::ios_base::in);
	
	if(timeTable)
	{
		std::vector<watchly::model::Task> tasks;
		
		std::string line;
		while(std::getline(timeTable, line))
		{
			watchly::model::Task task;

			std::stringstream row(line);
			std::string value;
			
			std::getline(row, value, ';');
			task.date = utils::time::parse(value);

			std::getline(row, value, ';');
			task.hours = std::stod(value);
			
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
	bool hasStoppedSomething = false;
	
	if(this->isRunning())
	{
		this->stopRunning();
		hasStoppedSomething = true;
	}
	
	if(this->hasBufferedTime())
	{
		this->resetBuffer();
		hasStoppedSomething = true;
	}
	
	if(!hasStoppedSomething)
	{
		throw std::logic_error("Cannot cancel a not running or buffered chronometer");
	}
}

std::chrono::duration<long, std::chrono::milliseconds::period> Core::pause() const
{
	std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> runningTime = this->getTimeSinceMarker();
	if(runningTime)
	{
		std::chrono::duration<long, std::chrono::milliseconds::period> duration = runningTime.value();
		
		std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> bufferedTime = this->getBufferedTime();
		if(bufferedTime)
		{
			duration += bufferedTime.value();
		}
		
		this->buffer(duration);
		this->stopRunning();
		
		return duration;
	}
	else
	{
		throw std::logic_error("Cannot pause a not running chronometer");
	}
}


void Core::putTask(double duration, const std::string& label, const std::chrono::system_clock::time_point& date) const
{
	std::ofstream timeTable = this->openWatchlyFile<std::ofstream>(Core::TIME_TABLE_FILE, std::ios_base::out | std::ios_base::app);
	
	if(timeTable)
	{
		auto time = std::chrono::system_clock::to_time_t(date);
		auto local = *std::localtime(&time);
		
		timeTable << std::put_time(&local, "%d/%m/%Y") << ";" << std::fixed << std::showpoint << std::setprecision(2) << duration << ";" << label << std::endl;	
	}
	else
	{
		throw std::ios_base::failure("Cannot write into time table file");
	}
}

double Core::generate(const std::string& path, const std::chrono::system_clock::time_point& from, const std::chrono::system_clock::time_point& to) const
{
	std::vector<watchly::model::Task> tasks = this->getHistory();	
	tasks.erase(
		std::remove_if(tasks.begin(), tasks.end(), [from, to](const watchly::model::Task& task) {
			return task.date < from || task.date > to;
		}),
		tasks.end()
	);
	
	std::ifstream tstream = this->openWatchlyFile<std::ifstream>(Core::TEMPLATE_FILE, std::ios_base::in);
	std::map<std::string, std::string> properties = this->getProperties();

	double amount = templating::Latex::generatePDF(tstream, path, tasks, properties);
	
	properties["lastinvoice"] = std::to_string(watchly::utils::invoice::nextInvoiceNumber(std::stoi(watchly::utils::map::getValueForKeyWithDefault<std::string, std::string>(properties, "lastinvoice", "0"))));
	this->updateProperties(properties); 
	
	return amount;
}

/**
 * Data Operations
 */
std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> Core::getBufferedTime() const
{
	std::optional<long> millis = this->read(Core::BUFFER_FILE);
	
	if(millis)
	{
		return std::chrono::duration<long, std::chrono::milliseconds::period>(millis.value());
	}
	else
	{
		return std::nullopt;
	}
}

std::map<std::string, std::string> Core::getProperties() const
{
	std::ifstream stream = this->openWatchlyFile<std::ifstream>(Core::PROPERTIES_FILE, std::ios_base::in);
	std::map<std::string, std::string> properties{};
	
	if(stream)
	{		
		std::string line;
		while(std::getline(stream, line))
		{
			std::stringstream keyValue(line);
			std::string key;
			std::string value;
			
			std::getline(keyValue, key, '=');
			std::getline(keyValue, value, '=');
			
			properties[key] = value;
		}
	}
	
	return properties;
}

bool Core::updateProperties(const std::map<std::string, std::string>& properties) const
{
	std::ofstream stream = this->openWatchlyFile<std::ofstream>(Core::PROPERTIES_FILE, std::ios_base::out);
	
	if(stream)
	{		
		for(std::map<std::string, std::string>::const_iterator it = properties.begin(); it != properties.end(); it++)
		{
			if(!it->first.empty())
			{
				stream << it->first << "=" << it->second << std::endl;
			}
		}
		
		return true;
	}
	else
	{
		return false;
	}
}

std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> Core::getTimeSinceMarker() const
{
	const std::optional<std::chrono::time_point<std::chrono::system_clock>> marker = this->getCurrentMarker();
	
	if(marker)
	{
		const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
		return std::chrono::duration_cast<std::chrono::milliseconds>(now - marker.value());
	}
	else
	{
		return std::nullopt;
	}
}

std::optional<std::chrono::time_point<std::chrono::system_clock>> Core::getCurrentMarker() const
{
	std::optional<long> millis = this->read(Core::MARKER_FILE);
	
	if(millis)
	{
		return std::chrono::time_point<std::chrono::system_clock>(std::chrono::milliseconds(millis.value()));
	}
	else
	{
		return std::nullopt;
	}
}

void Core::buffer(const std::chrono::duration<long, std::chrono::milliseconds::period>& duration) const
{
	bool success = this->write(Core::BUFFER_FILE, duration.count());
	
	if(!success)
	{
		throw std::runtime_error("Cannot buffer running chronometer"); 
	}
}

void Core::stopRunning() const
{
	bool success = this->removeWatchlyFile(Core::MARKER_FILE);
	
	if(!success)
	{
		throw std::runtime_error("Cannot stop running chronometer");
	}
}

void Core::resetBuffer() const
{
	bool success = this->removeWatchlyFile(Core::BUFFER_FILE);
	
	if(!success)
	{
		throw std::runtime_error("Cannot reset running chronometer");
	}
}

bool Core::isRunning() const
{
	return this->watchlyFileExists(Core::MARKER_FILE);
}

bool Core::hasBufferedTime() const
{
	return this->watchlyFileExists(Core::BUFFER_FILE);
}

/**
 * File Operations
 */

bool Core::watchlyFileExists(const std::string& file) const
{
	return std::filesystem::exists(file);
}

bool Core::removeWatchlyFile(const std::string& file) const
{	
	try
	{
		return std::filesystem::remove(file);
	}
	catch(const std::exception& e)
	{
		return false;
	}
}

std::optional<long> Core::read(const std::string& file) const
{
	try
	{
		long millis = 0;
		
		std::ifstream stream = this->openWatchlyFile<std::ifstream>(file, std::ios_base::in);
		stream >> millis;
		stream.close();
		
		return millis;
	}
	catch(const std::exception& e)
	{
		return std::nullopt;
	}
}

bool Core::write(const std::string& file, long value) const
{
	try
	{
		std::ofstream stream = this->openWatchlyFile<std::ofstream>(file, std::ios_base::out);		
		stream << value;
		stream.close();
		
		return true;
	}
	catch(const std::exception& e)
	{
		return false;
	}
}
