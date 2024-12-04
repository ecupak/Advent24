#include "shared.h"

#include <iostream>
#include <iterator>
#include <regex>


int main()
{
	shared::timer();

	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	unsigned int total{ 0 };

	// Controls if do (1) or don't (0) was last found. Starts as do (1).		
	unsigned int modifier{ 1 };

	// Evaluate lines in list.
	for (const auto& line : list)
	{
		if (line.empty()) continue;

		// Get a do() or a don't() or instructions.
		std::regex instruction_regex{ "(do\\(\\)|don't\\(\\)|mul\\(\\d{1,3},\\d{1,3}\\))" };
		auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
		auto instruction_end{ std::sregex_iterator() };

		for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
		{
			auto instructions{ (*i).str() };			
			
			if (instructions == "do()")
			{
				modifier = 1;
				continue;
			}
			else if (instructions == "don't()")
			{
				modifier = 0;
				continue;
			}

			// Get all the numbers, grouped up to 3 digits each.
			std::regex num_regex{ "(\\d{1,3})" };
			auto num_begin{ std::sregex_iterator(instructions.begin(), instructions.end(), num_regex) };
			auto num_end{ std::sregex_iterator() };

			std::vector<unsigned int> values;

			for (std::sregex_iterator n{ num_begin }; n != num_end; ++n)
			{
				values.push_back(std::stoi((*n).str()));
			}

			// Multiply and add result.
			if (values.size() == 2)
			{
				total += values[0] * values[1] * modifier;
			}
		}
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << '\n' << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);
}