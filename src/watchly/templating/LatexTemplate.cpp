#include "LatexTemplate.hpp"
#include <sstream>
#include <fstream>
#include <string>
#include <numeric>
#include <filesystem>
#include "../utils/TimeUtils.hpp"
#include "../utils/MapUtils.hpp"
#include "../utils/InvoiceUtils.hpp"

using namespace watchly::templating;

std::string LatexTemplate::TEMPORARY_DIRECTORY = "./temporary/";
std::string LatexTemplate::TEMPORARY_TEX_FILE = LatexTemplate::TEMPORARY_DIRECTORY + "temp.tex";
std::string LatexTemplate::TEMPORARY_PDF_FILE = LatexTemplate::TEMPORARY_DIRECTORY + "temp.pdf";

double LatexTemplate::generatePDF(std::istream& tstream, const std::string& destination, const std::vector<Task>& tasks, const std::map<std::string, std::string>& properties)
{
	std::filesystem::create_directory(LatexTemplate::TEMPORARY_DIRECTORY );
	
	double amount = LatexTemplate::templatize(tstream, LatexTemplate::TEMPORARY_TEX_FILE, tasks, properties);
	
	system((std::string("pdflatex -interaction=nonstopmode -output-directory=") + LatexTemplate::TEMPORARY_DIRECTORY + " " + LatexTemplate::TEMPORARY_TEX_FILE + " > /dev/null").c_str());
	std::filesystem::rename(LatexTemplate::TEMPORARY_PDF_FILE, destination);
	
	std::filesystem::remove_all(LatexTemplate::TEMPORARY_DIRECTORY );
	
	return amount;
}
double LatexTemplate::templatize(std::istream& tstream, const std::string& destination, const std::vector<Task>& tasks, const std::map<std::string, std::string>& properties)
{
	double hourlyRate = std::stod(watchly::utils::map::getValueForKeyWithDefault<std::string, std::string>(properties, "hourlyrate", "1"));

	std::map<std::string, std::string> parameters(properties);
			
	int invoiceNumber = watchly::utils::invoice::nextInvoiceNumber(std::stoi(watchly::utils::map::getValueForKeyWithDefault<std::string, std::string>(properties, "lastinvoice", "0")));
	parameters["invoicenumber"] = std::to_string(invoiceNumber);
	parameters["tasks"] = LatexTemplate::toTable(tasks, hourlyRate);
	
	double totalHours = std::accumulate(tasks.begin(), tasks.end(), 0.0, [](double accumulator, const Task& task) {
		return accumulator + task.hours;
	});
	parameters["totalhours"] = watchly::utils::invoice::format(totalHours);
	
	double totalAmount = totalHours * hourlyRate;
	parameters["totalamount"] = watchly::utils::invoice::format(totalAmount);
	
	std::ofstream out(destination);
	if(out)
	{
		std::string line;
		while(std::getline(tstream, line))
		{
			bool isParameter = false;
			for(const std::pair<std::string, std::string>& parameter : parameters)
			{
				if(line == std::string("\\newcommand{\\") + parameter.first + "}{}")
				{
					out << "\\newcommand{\\" << parameter.first << "}{" << parameter.second << "}\n";
					isParameter = true;
				}
			}
			
			if(!isParameter)
			{
				out << line << "\n";
			}
		}
		
		return totalAmount;
	}
	else
	{
		throw std::ios_base::failure(std::string("Cannot open ") + destination);
	}
}

std::string LatexTemplate::toTable(const std::vector<Task>& tasks, double hourlyRate)
{
	std::ostringstream latex;
	
	for(const Task& task : tasks)
	{
		latex << task.label << " & " << watchly::utils::TimeUtils::format(task.date) << " & " << watchly::utils::invoice::format(hourlyRate) << " € & " << watchly::utils::invoice::format(task.hours) << " h & " << watchly::utils::invoice::format(task.hours * hourlyRate) << " € \\\\";
	}
	
	return latex.str();
}
