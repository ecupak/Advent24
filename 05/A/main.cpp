#include "shared.h"

#include <iostream>

#include <iostream>
#include <iterator>
#include <regex>

#include <unordered_map>
#include <functional>


void addRule(const std::string& line, std::unordered_map<size_t, std::string>& rules, std::hash<std::string>& hasher);

unsigned int checkOrder(const std::string& line, std::unordered_map<size_t, std::string>& rules, std::hash<std::string>& hasher);

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
	std::unordered_map<size_t, std::string> rules;

	bool reading_page_rules{ true };

	for (const auto& line : list)
	{
		if (line.empty()) reading_page_rules = false;

		// Build rules.
		if (reading_page_rules)
		{
			std::regex rule_regex{ "(\\d+\\|\\d+)" };
			auto rule_begin{ std::sregex_iterator(line.begin(), line.end(), rule_regex) };
			auto rule_end{ std::sregex_iterator() };

			if ( rule_begin == rule_end)
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


void addRule(const std::string& line, std::unordered_map<size_t, std::string>& rules, std::hash<std::string>& hasher)
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
	{
		{
			std::string regex_rule{ page_numbers[1] + ".+" + page_numbers[0] };
			if (rules.find(hasher(page_numbers[0])) == rules.end())
			{
				rules[hasher(page_numbers[0])] = regex_rule;
			}
			else
			{
				// Extend rule.
				rules[hasher(page_numbers[0])] += "|" + regex_rule;
			}
		}
	}
}


unsigned int checkOrder(const std::string& line, std::unordered_map<size_t, std::string>& rules, std::hash<std::string>& hasher)
{
	std::vector<unsigned int> values;

	std::regex instruction_regex{ "(\\d+)" };
	auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
	auto instruction_end{ std::sregex_iterator() };

	// For each number, check it's rules.
	for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
	{
		std::string page_number{ (*i).str() };
		std::string page_rules{ rules[hasher(page_number)] };
		
		if (!page_rules.empty())
		{
			std::regex rule_regex{ "(" + page_rules + ")" };
			auto rule_begin{ std::sregex_iterator(line.begin(), line.end(), rule_regex) };
			auto rule_end{ std::sregex_iterator() };

			// Found an invalid rule.
			if (rule_begin != rule_end)
			{
				return 0;
			}
		}

		values.push_back(std::stoi(page_number));
	}

	size_t mid_idx{ (values.size() - 1u) / 2u };
	return values[mid_idx];
}