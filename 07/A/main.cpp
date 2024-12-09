// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 0 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

// Regex.
#include <iostream>
#include <iterator>
#include <regex>

#include <numeric>


int main()
{
	shared::timer();

#if VISUAL_MODE == 0
	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list, true) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	size_t total{ 0 };

	// std::vector<int> values;

	enum Instruction
	{
		Add = 0,
		Multiply = 1,
	};

	for (auto& line : list)
	{
		if (line.empty()) continue;

		std::regex instruction_regex{ "(\\d+)" };
		auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
		auto instruction_end{ std::sregex_iterator() };

		std::vector<size_t> nums;

		for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
		{
			nums.push_back(static_cast<size_t>(std::stoull((*i).str())));
		}

		// Operator count.
		size_t op_count{ nums.size() - 2 };
		size_t max_op_combo{ 1 };
		for (size_t i{ 1 }; i < op_count; ++i)
		{
			max_op_combo = (max_op_combo << 1) | 1;
		}

		// Test all combos.
		size_t answer{ nums[0] };
		bool was_answered{ false };
		for (size_t op_combo{ 0 }; op_combo < max_op_combo + 1; ++op_combo)
		{
			size_t sum{ nums[1] };
			
			// Advance from left to right, applying operator.
			for (size_t op{ 0 }; op < op_count; ++op)
			{
				size_t op_bit{ static_cast<size_t>(1ull << op) };
				size_t op_instruction{ (op_combo & op_bit) >> op };
				switch (op_instruction)
				{
					case Instruction::Add:
						sum += nums[op + 2];
						break;
					case Instruction::Multiply:
						sum *= nums[op + 2];
						break;
				}
			}

#ifdef _DEBUG
			std::cout << answer << ": " << nums[1];

			for (size_t op{ 0 }; op < op_count; ++op)
			{
				size_t op_bit{ static_cast<size_t>(1ull << op) };
				size_t op_instruction{ (op_combo & op_bit) >> op };
				switch (op_instruction)
				{
					case Instruction::Add:
						std::cout << " + " << nums[op + 2];
						break;
					case Instruction::Multiply:
						std::cout << " * " << nums[op + 2];
						break;
				}
			}

			if (sum == answer)
			{
				was_answered = true;
				std::cout << " CORRECT";
			}

			std::cout << std::endl;
#else
			if (sum == answer)
			{
				total += answer;
				break;
			}
#endif
		}

#ifdef _DEBUG
		if (was_answered)
		{
			total += answer;
		}
		std::cout << "\n";
#endif
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
#else
	// Visualize the puzzle using an extended class and SFML loop.
	
	// PUZZLE DATA //

	class Puzzle : public vizit::Challenge
	{
	public:
		void Init() override
		{
			// Sets SFML font / text data.
			m_font_size = 10;
			Challenge::Init();

			for (auto& line : list)
			{
				// shared::getSpaceDelimitedValuesFromLine<int>(line, values);
			}

			m_dir = shared::Directions::UpIdx;
			m_direction = shared::directions[m_dir];
		}

		// Drawing to the window.
		void Render()
		{
			m_text.setString("Exaple text.");
			m_text.setFillColor(sf::Color(150, 150, 150, 255));
			m_text.setFillColor(sf::Color::Green);

			int x = y = 20;
			m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
			m_window->draw(m_text);
		}

		// Return true when puzzle is solved. Will pause process.
		bool Tick(float, bool can_advance) override 
		{		
			return false;
		}

		// std::vector<int> values;
		
	};

	Puzzle puzzle{};

	// Can optionally pass the screen width and height to use.
	// Default will use the config settings.
	// Challenge class contains width and height of input, based on first line.
	return vizit::run(puzzle);
	//return vizit::run(puzzle, 400, 600);

#endif
	shared::timer(true);
}