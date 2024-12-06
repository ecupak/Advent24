#include "shared.h"

#include <iostream>

#include <iostream>
#include <iterator>
#include <regex>

#include <unordered_map>
#include <functional>

using RuleList = std::unordered_map < size_t, std::vector<std::pair<std::string, std::string>>> ;

void addRule(std::string& line, RuleList& rules, std::hash<std::string>& hasher);

unsigned int checkOrder(std::string& line, RuleList& rules, std::hash<std::string>& hasher);


int main()
{
	shared::timer();

	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	unsigned int total{ 0 };

	std::hash<std::string> hasher;
	RuleList rules;

	bool reading_page_rules{ true };

	for (auto& line : list)
	{
		if (line.empty()) reading_page_rules = false;

		// Build rules.
		if (reading_page_rules)
		{
			std::regex rule_regex{ "(\\d+\\|\\d+)" };
			auto rule_begin{ std::sregex_iterator(line.begin(), line.end(), rule_regex) };
			auto rule_end{ std::sregex_iterator() };

			if (rule_begin == rule_end)
			{
				reading_page_rules = false;
				total += checkOrder(line, rules, hasher);
			}
			else
			{
				addRule(line, rules, hasher);
			}
		}

		// Evaluate lines.
		else
		{
			total += checkOrder(line, rules, hasher);
		}
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);
}


void addRule(std::string& line, RuleList& rules, std::hash<std::string>& hasher)
{
	std::regex instruction_regex{ "(\\d+)" };
	auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
	auto instruction_end{ std::sregex_iterator() };

	std::vector<std::string> page_numbers;

	for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
	{
		page_numbers.push_back((*i).str());
	}

	// For each rule, store the invalid form (i.e., "43|97" = "(97.+43)").
	// The we can search for that against the manual pages later.		
	rules[hasher(page_numbers[0])].emplace_back(page_numbers[1], page_numbers[0]);;
}


unsigned int checkOrder(std::string& line, RuleList& rules, std::hash<std::string>& hasher)
{
	// Deconstruct string into vector.
	std::vector<std::string> str_values;
	{
		std::istringstream str{ line };

		while (!str.eof())
		{
			std::string value;
			std::getline(str, value, ',');
			str_values.push_back(value);
		}
	}

	bool is_invalid{ true };

	size_t counter{ 0 };
	while (is_invalid)
	{
		is_invalid = false;

		std::regex instruction_regex{ "(\\d+)" };
		auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
		auto instruction_end{ std::sregex_iterator() };

		// For each number, check it's rules.		
		for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
		{
			std::string page_number{ (*i).str() };
			auto& page_rules{ rules[hasher(page_number)] };

			for (auto& rule : page_rules)
			{
				std::regex rule_regex{ "(" + rule.first + ".+" + rule.second + ")" };
				auto rule_begin{ std::sregex_iterator(line.begin(), line.end(), rule_regex) };
				auto rule_end{ std::sregex_iterator() };

				// Found an invalid rule.
				for (std::sregex_iterator k{ rule_begin }; k != rule_end; ++k)
				{
					is_invalid = true;

					// Find wrong indices and swap.
					size_t i1{ 0 }, i2{ 0 };

					for (int s{ 0 }; s < str_values.size(); ++s)
					{
						if (str_values[s] == rule.first) i1 = s;
						if (str_values[s] == rule.second) i2 = s;
					}

					std::swap(str_values[i1], str_values[i2]);

					// Rebuild line.
					line = "";					
					for (int n{ 0 }; n < str_values.size(); ++n)
					{
						line += str_values[n] + (n + 1 != str_values.size() ? "," : "");
					}
					break;
				}
			}
		}

		// Was correct on first check - discard.
		if (counter == 0 && !is_invalid)
		{
			return 0;
		}

		++counter;
	} // While invalid keep trying to fix it.

	size_t mid_idx{ (str_values.size() - 1u) / 2u };
	return std::stoi(str_values[mid_idx]);
}