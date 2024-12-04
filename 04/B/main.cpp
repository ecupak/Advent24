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
			if (line[k] == 'A')
			{
				char prev{ ' ' };
				char curr{ ' ' };

				// NE
				try {
					prev = list.at(i - 1).at(k + 1);
					if (prev != 'M' && prev != 'S') continue;
				}
				catch (...) { continue; }

				// SW
				try {
					curr = list.at(i + 1).at(k - 1);
					if (curr != 'M' && curr != 'S') continue;
					if (curr == prev) continue;
				}
				catch (...) { continue; }


				// SE
				try {
					prev = list.at(i + 1).at(k + 1);
					if (prev != 'M' && prev != 'S') continue;
				}
				catch (...) { continue; }


				// NW
				try {
					curr = list.at(i - 1).at(k - 1);
					if (curr != 'M' && curr != 'S') continue;
					if (curr == prev) continue;
					total += 1;
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