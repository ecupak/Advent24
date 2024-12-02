#include "shared.h"


int main()
{
	shared::timer();

	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (shared::parseInputFile(list) == 1)
	{
		return 1;
	}

	// SOLVE PUZZLE //

	// Build list of left and right values.
	for (const auto& line : list)
	{
		if (line.empty()) continue;

		std::vector<unsigned int> values;

		//shared::getSpaceDelimitedValuesFromLine<unsigned int>(line, values);
	}


	// Answer!
	//std::cout << total << std::endl;

	shared::timer(true);
}