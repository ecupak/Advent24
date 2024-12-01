#include "shared.h"

#include <numeric> 
#include <unordered_map>


int main()
{
	// PARSE INPUT FILE //

	std::vector<std::string> list;

	if (shared::parseInputFile(list) == 1)
	{
		return 1;
	}

	// SOLVE PUZZLE //

	std::vector<unsigned int> left_values;
	std::vector<unsigned int> right_values;

	// Build list of left and right values.
	for (const auto& line : list)
	{
		if (line.empty()) continue;

		std::vector<unsigned int> values;

		shared::getSpaceDelimitedValuesFromLine<unsigned int>(line, values);

		left_values.push_back(values[0]);
		right_values.push_back(values[1]);
	}

	// Sort both ascending.
	std::sort(left_values.begin(), left_values.end());
	std::sort(right_values.begin(), right_values.end());

	// Find similarities.
	unsigned int similarity_score{ 0 };

	// ... only need to find values once per number.
	std::unordered_map<unsigned int, unsigned int> lookup;

	for (auto value : left_values)
	{
		if (auto iter{ lookup.find(value) }; iter != lookup.end())
		{
			similarity_score += lookup[value];
		}
		else
		{
			unsigned int score{ 0 };

			for (auto other_value : right_values)
			{
				if (value == other_value)
				{
					score += other_value;
				}
			}

			lookup[value] = score;

			similarity_score += score;
		}
	}

	// Answer!
	std::cout << similarity_score << std::endl;
}