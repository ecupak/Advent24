#include "shared.h"

#include <numeric> 


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

	std::vector<unsigned int> distances;

	// Compare and get distances.
	// Avoiding abs() because unsigned int values may underflow if minuend is less than subtrahend.
	for (int i{ 0 }; i < left_values.size(); ++i)
	{
		if (left_values[i] >= right_values[i])
		{
			distances.push_back(left_values[i] - right_values[i]);
		}
		else
		{
			distances.push_back(right_values[i] - left_values[i]);
		}
	}

	// Sum up values.
	auto total{ std::reduce(distances.begin(), distances.end()) };

	// Answer!
	std::cout << total << std::endl;
}