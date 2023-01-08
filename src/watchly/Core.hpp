#ifndef __WATCHLY_CORE__
#define __WATCHLY_CORE__

#include <string>
#include <chrono>
#include <optional>
#include <vector>
#include <fstream>
#include <filesystem>
#include "Task.hpp"

namespace watchly
{
	class Core
	{
		private:
			static const std::string WATCHLY_DIRECTORY;
			static const std::string TIME_TABLE_FILE;
			static const std::string MARKER_FILE;
			
		public:
			Core() = default;
			
			void start() const;
			double stop(const std::string& label) const;
			std::vector<Task> getHistory() const;
			
		private:
			std::optional<std::chrono::time_point<std::chrono::system_clock>> getCurrentMarker() const;
			void insertEntry(double duration, const std::string& label) const;
			
			template<class T>
			T openWatchlyFile(const std::string& file, std::ios_base::openmode mode = std::fstream::in | std::fstream::out) const
			{
				std::filesystem::create_directories(Core::WATCHLY_DIRECTORY);
				
				T stream;
				stream.open(Core::WATCHLY_DIRECTORY + file, mode);
				
				return stream;
			}
			
			void removeWatchlyFile(const std::string& file) const;
	};
}

#endif
