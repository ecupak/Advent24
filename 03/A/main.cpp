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

	// Evaluate lines in list.
	for (const auto& line : list)
	{
		if (line.empty()) continue;
				
		// Get all the instructions = mul(x[y[z]],x[y[z]])
		std::regex instruction_regex{ "(mul\\(\\d{1,3},\\d{1,3}\\))" };
		auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
		auto instruction_end{ std::sregex_iterator() };

		for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
		{
			auto instructions{ (*i).str() };
			std::cout << instructions << std::endl;
			
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
				std::cout << values[0] << " * " << values[1] << std::endl;

				total += values[0] * values[1];
			}
		}
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << '\n' << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);
}