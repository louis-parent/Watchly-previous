#ifndef __LATEX_TEMPLATE_H__
#define __LATEX_TEMPLATE_H__

#include <istream>
#include <vector>
#include <string>
#include <map>
#include "../model/Task.hpp"

namespace watchly::templating
{
	class Latex
	{
		private:
			static const std::string TEMPORARY_DIRECTORY;
			static const std::string TEMPORARY_TEX_FILE;
			static const std::string TEMPORARY_PDF_FILE;
		
		public:
			static double generatePDF(std::istream& tstream, const std::string& destination, const std::vector<watchly::model::Task>& tasks, const std::map<std::string, std::string>& properties);
		
		private:
			static double templatize(std::istream& tstream, const std::string& destination, const std::vector<watchly::model::Task>& tasks, const std::map<std::string, std::string>& properties);
			static std::string toTable(const std::vector<watchly::model::Task>& tasks, double hourlyRate);
	};
}

#endif
