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
			static const std::string BUFFER_FILE;
			
		public:
			Core() = default;
			
			void start() const;
			double stop(const std::string& label) const;
			std::vector<Task> getHistory() const;
			void cancel() const;
			void pause() const;
			
		private:
			/**
			 * Data Operations
			 */			
			std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> getBufferedTime() const;
			std::optional<std::chrono::duration<long, std::chrono::milliseconds::period>> getTimeSinceMarker() const;
			std::optional<std::chrono::time_point<std::chrono::system_clock>> getCurrentMarker() const;
			
			void buffer(const std::chrono::duration<long, std::chrono::milliseconds::period>& duration) const;
			void stopRunning() const;
			void resetBuffer() const;
			
			bool isRunning() const;
			bool hasBufferedTime() const;
			
			void insertEntry(double duration, const std::string& label) const;
			
			/**
			 * File Operations
			 */
			template<class T>
			T openWatchlyFile(const std::string& file, std::ios_base::openmode mode = std::fstream::in | std::fstream::out) const
			{
				std::filesystem::create_directories(Core::WATCHLY_DIRECTORY);
				
				T stream;
				stream.open(Core::WATCHLY_DIRECTORY + file, mode);
				
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
