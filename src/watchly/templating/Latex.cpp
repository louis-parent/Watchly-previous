#include "Latex.hpp"
#include <sstream>
#include <fstream>
#include <string>
#include <numeric>
#include <filesystem>
#include "../utils/TimeUtils.hpp"
#include "../utils/MapUtils.hpp"
#include "../utils/InvoiceUtils.hpp"

using namespace watchly::templating;

const std::string Latex::TEMPORARY_DIRECTORY = "./temporary/";
const std::string Latex::TEMPORARY_TEX_FILE = Latex::TEMPORARY_DIRECTORY + "temp.tex";
const std::string Latex::TEMPORARY_PDF_FILE = Latex::TEMPORARY_DIRECTORY + "temp.pdf";

double Latex::generatePDF(std::istream& tstream, const std::string& destination, const std::vector<watchly::model::Task>& tasks, const std::map<std::string, std::string>& properties)
{
	std::filesystem::create_directory(Latex::TEMPORARY_DIRECTORY );
	
	double amount = Latex::templatize(tstream, Latex::TEMPORARY_TEX_FILE, tasks, properties);
	
	system((std::string("pdflatex -interaction=nonstopmode -output-directory=") + Latex::TEMPORARY_DIRECTORY + " " + Latex::TEMPORARY_TEX_FILE + " > /dev/null").c_str());
	std::filesystem::rename(Latex::TEMPORARY_PDF_FILE, destination);
	
	std::filesystem::remove_all(Latex::TEMPORARY_DIRECTORY );
	
	return amount;
}
double Latex::templatize(std::istream& tstream, const std::string& destination, const std::vector<watchly::model::Task>& tasks, const std::map<std::string, std::string>& properties)
{
	double hourlyRate = std::stod(watchly::utils::map::getValueForKeyWithDefault<std::string, std::string>(properties, "hourlyrate", "1"));

	std::map<std::string, std::string> parameters(properties);
			
	int invoiceNumber = watchly::utils::invoice::nextInvoiceNumber(std::stoi(watchly::utils::map::getValueForKeyWithDefault<std::string, std::string>(properties, "lastinvoice", "0")));
	parameters["invoicenumber"] = std::to_string(invoiceNumber);
	parameters["tasks"] = Latex::toTable(tasks, hourlyRate);
	
	double totalHours = std::accumulate(tasks.begin(), tasks.end(), 0.0, [](double accumulator, const watchly::model::Task& task) {
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

std::string Latex::toTable(const std::vector<watchly::model::Task>& tasks, double hourlyRate)
{
	std::ostringstream latex;
	
	for(const watchly::model::Task& task : tasks)
	{
		latex << task.label << " & " << watchly::utils::time::format(task.date) << " & " << watchly::utils::invoice::format(hourlyRate) << " € & " << watchly::utils::invoice::format(task.hours) << " h & " << watchly::utils::invoice::format(task.hours * hourlyRate) << " € \\\\";
	}
	
	return latex.str();
}
