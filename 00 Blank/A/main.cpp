#include "shared.h"

#include <iostream>


int main()
{
	shared::timer();

	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	unsigned int total{ 0 };

	// Evaluate lines in list.
	for (const auto& line : list)
	{
		if (line.empty()) continue;
	
		//shared::getSpaceDelimitedValuesFromLine<unsigned int>(line, values);
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);
}