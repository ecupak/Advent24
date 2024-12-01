#include "text_reader.h"


int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (shared::parseInputFile(list) == 1)
	{
		return 1;
	}

	// SOLVE PUZZLE //

	std::vector<int> values;



}