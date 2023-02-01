#ifndef __WATCHLY_CORE__
#define __WATCHLY_CORE__

#include <string>
#include <chrono>
#include <optional>
#include <vector>
#include <fstream>
#include <filesystem>
#include <map>
#include "Task.hpp"

namespace watchly
{
	class Core
	{
		private:
			static const std::string WATCHLY_DIRECTORY;
			static const std::string TIME_TABLE_FILE;
			static const std::string MARKER_FILE;
			static const std::string BUFFER_FILE;
			static const std::string TEMPLATE_FILE;
			static const std::string PROPERTIES_FILE;
			
		public:
			Core() = default;
			
			void start() const;
			double stop(const std::string& label) const;
			std::vector<Task> getHistory() const;
			void cancel() const;
			std::chrono::duration<long, std::chrono::milliseconds::period> pause() const;
			void putTask(double duration, const std::string& label) const;
			double generate(const std::string& path, const std::chrono::system_clock::time_point& from, const std::chrono::system_clock::time_point& to = std::chrono::system_clock::now()) const;
			
			bool isRunning() const;
			bool hasBufferedTime() const;
			
			std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> getBufferedTime() const;
			std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> getTimeSinceMarker() const;

		private:		
			/**
			 * Data Operations
			 */			
			std::optional<std::chrono::time_point<std::chrono::system_clock>> getCurrentMarker() const;
			
			void buffer(const std::chrono::duration<long, std::chrono::milliseconds::period>& duration) const;
			void stopRunning() const;
			void resetBuffer() const;
			
			std::map<std::string, std::string> getProperties() const;
			
			/**
			 * File Operations
			 */
			template<class T>
			T openWatchlyFile(const std::string& file, std::ios_base::openmode mode = std::fstream::in | std::fstream::out) const
			{
				std::filesystem::create_directories(Core::WATCHLY_DIRECTORY);
				
				T stream;
				stream.open(file, mode);
				
				if(stream)
				{
					return stream;
				}
				else
				{
					throw std::ios_base::failure(std::string("Cannot open ") + file);
				}
			}
			
			bool watchlyFileExists(const std::string& file) const;
			bool removeWatchlyFile(const std::string& file) const;
			
			std::optional<long> read(const std::string& file) const;
			bool write(const std::string& file, long value) const;
	};
}

#endif
