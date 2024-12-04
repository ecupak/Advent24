#include "shared.h"

#include <iostream>
#include <iostream>
#include <iterator>
#include <regex>


//bool findMAS(const std::string& line);


int main()
{
	shared::timer();

	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	unsigned int total{ 0 };

	for (int i{ 0 }; i < list.size(); ++i)
	{
		std::string& line{ list[i] };

		for (int k{ 0 }; k < line.size(); ++k)
		{
			if (line[k] == 'X')
			{
				// EAST
				try {
					if (line.at(k + 1) == 'M'
						&& line.at(k + 2) == 'A'
						&& line.at(k + 3) == 'S') total += 1;
				}
				catch (...) { }

				// WEST
				try {
					if (line.at(k - 1) == 'M'
						&& line.at(k - 2) == 'A'
						&& line.at(k - 3) == 'S') total += 1;
				}
				catch (...) { }

				// NORTH
				try {
					if (list.at(i - 1).at(k) == 'M'
						&& list.at(i - 2).at(k) == 'A'
						&& list.at(i - 3).at(k) == 'S') total += 1;
				}
				catch (...) { }

				// SOUTH
				try {
					if (list.at(i + 1).at(k) == 'M'
						&& list.at(i + 2).at(k) == 'A'
						&& list.at(i + 3).at(k) == 'S') total += 1;
				}
				catch (...) { }

				// NE
				try {
					if (list.at(i - 1).at(k + 1) == 'M'
						&& list.at(i - 2).at(k + 2) == 'A'
						&& list.at(i - 3).at(k + 3) == 'S') total += 1;
				}
				catch (...) { }

				// SE
				try {
					if (list.at(i + 1).at(k + 1) == 'M'
						&& list.at(i + 2).at(k + 2) == 'A'
						&& list.at(i + 3).at(k + 3) == 'S') total += 1;
				}
				catch (...) { }

				// SW
				try {
					if (list.at(i + 1).at(k - 1) == 'M'
						&& list.at(i + 2).at(k - 2) == 'A'
						&& list.at(i + 3).at(k - 3) == 'S') total += 1;
				}
				catch (...) { }

				// NW
				try {
					if (list.at(i - 1).at(k - 1) == 'M'
						&& list.at(i - 2).at(k - 2) == 'A'
						&& list.at(i - 3).at(k - 3) == 'S') total += 1;
				}
				catch (...) { }
			}

		}
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);
}