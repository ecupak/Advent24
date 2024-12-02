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

	int total{ 0 };

	// Get levels per report.
	for (const auto& line : list)
	{
		if (line.empty()) continue;
		
		std::vector<unsigned int> levels;
		
		shared::getSpaceDelimitedValuesFromLine<unsigned int>(line, levels);

		// Check if descending or ascending.
		int sort_order{ 0 };
		enum
		{
			DES = -1,
			ASC = 1
		};

		std::vector<unsigned int> levels_sorted = levels;

		// ... Ascending check.
		std::sort(levels_sorted.begin(), levels_sorted.end());
		if (levels == levels_sorted) 
		{
			sort_order = ASC;
		}
		else
		{
			// ... Descending check.
			std::sort(levels_sorted.begin(), levels_sorted.end(), std::greater());
			if (levels == levels_sorted)
			{
				sort_order = DES;
			}
		}

		// Not in any order, skip.
		if (sort_order == 0) continue;
		
		// Confirm gradual changes.
		bool is_gradual_change{ true };
				
		for (size_t i{ 0 }; i < levels.size() - 1; ++i)
		{
			size_t difference{ 0 };

			if (sort_order == ASC)
			{
				difference = levels[i + 1] - levels[i];
			}
			else
			{
				difference = levels[i] - levels[i + 1];
			}
			
			if (difference < 1 || difference > 3)
			{
				is_gradual_change = false;
				break;
			}
		}

		if (is_gradual_change) total += 1;
	}

	// Answer!
	std::cout << total << std::endl;

	shared::timer(true);
}