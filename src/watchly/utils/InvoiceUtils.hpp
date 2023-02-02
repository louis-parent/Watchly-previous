#ifndef __INVOICE_UTILS_H__
#define __INVOICE_UTILS_H__

#include <string>
#include <ctime>
#include <sstream>

namespace watchly::utils::invoice
{
	struct DecimalComma : std::numpunct<char>
	{
    	char do_decimal_point() const { return ','; }
	};
	
	inline int nextInvoiceNumber(int lastInvoice)
	{
		int maxInvoicePerYearCount = 10000;
		
		time_t current_time = std::time(NULL);
		int currentYear = 1970 + static_cast<int>(current_time / 31537970);
		int twoDigitYear = currentYear - ((currentYear / 100) * 100);
		
    	int previousNumber = (twoDigitYear * maxInvoicePerYearCount);
    	
    	if((lastInvoice / maxInvoicePerYearCount) == twoDigitYear)
    	{
    		previousNumber = lastInvoice;
    	}
    	
		return previousNumber + 1;
	}
	
	inline std::string format(double value)
	{
		std::ostringstream formater;
		formater.imbue(std::locale(formater.getloc(), new DecimalComma));
		formater << std::fixed << std::showpoint << std::setprecision(2) << value;
		return formater.str();
	}
}

#endif
