#ifndef __LATEX_TEMPLATE_H__
#define __LATEX_TEMPLATE_H__

#include <istream>
#include <vector>
#include <string>
#include <map>
#include "../Task.hpp"

namespace watchly::templating
{
	class LatexTemplate
	{
		public:
			static std::string TEMPORARY_DIRECTORY;
			static std::string TEMPORARY_TEX_FILE;
			static std::string TEMPORARY_PDF_FILE;
			
			static double generatePDF(std::istream& tstream, const std::string& destination, const std::vector<Task>& tasks, const std::map<std::string, std::string>& properties);
			static double templatize(std::istream& tstream, const std::string& destination, const std::vector<Task>& tasks, const std::map<std::string, std::string>& properties);
			static std::string toTable(const std::vector<Task>& tasks, double hourlyRate);
	};
}

#endif
