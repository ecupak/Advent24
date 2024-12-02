#include "shared.h"


bool IsSafe(std::vector<unsigned int>& levels);
bool CheckReport(std::vector<unsigned int>& report);

int main()
{
	shared::timer();

	// PARSE INPUT FILE //

	std::vector<std::string> list;

	int parse_value{ 2 };
	if (parse_value = shared::parseInputFile(list); parse_value == 2)
	{
		return 2;
	}

	// SOLVE PUZZLE //

	int total{ 0 };

	// Get levels per report.
	for (const auto& line : list)
	{
		if (line.empty()) continue;

		std::vector<unsigned int> report;

		shared::getSpaceDelimitedValuesFromLine<unsigned int>(line, report);

		// Check report - safe / unsafe?
		if (bool is_safe{ CheckReport(report) })
		{
			total += 1;
		}
		else
		{
			std::reverse(report.begin(), report.end());

			if (bool is_safe{ CheckReport(report) })
				total += 1;
		}
	}

	// Answer!
	std::cout << total << std::endl;

	// Warn if using a test file and not real input.
	if (parse_value == 1)
		std::cout << "WARNING - USING TEST FILE" << std::endl;

	shared::timer(true);
}

bool CheckReport(std::vector<unsigned int>& report)
{
	bool is_safe{ IsSafe(report) };

	// Ascending check.
	if (!is_safe)
	{
		// i = element to remove.
		for (int i{ 0 }; i < report.size(); ++i)
		{
			std::vector<unsigned int> edited_report;

			// k = index over levels.
			for (int k{ 0 }; k < report.size(); ++k)
			{
				if (k == i) continue;

				edited_report.push_back(report[k]);
			}

			if (is_safe = IsSafe(edited_report))
			{
				return is_safe;
			}
		}
	}
	else
	{
		return is_safe;
	}

	return is_safe;
}

bool IsSafe(std::vector<unsigned int>& report)
{
	for (int i{ 0 }; i < report.size() - 1; ++i)
	{
		if (report[i] >= report[i + 1])
		{
			return false;
		}

		unsigned int difference{ report[i + 1] - report[i] };

		if (difference < 1 || difference > 3)
		{
			return false;
		}
	}

	return true;
}