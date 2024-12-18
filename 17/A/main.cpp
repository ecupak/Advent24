// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 0 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE false


#include "shared.h"

#include <iostream>
#include <vector>

#include <cassert>

// Regex.
#include <iostream>
#include <iterator>
#include <regex>


size_t reg_a{ 0 };
size_t reg_b{ 0 };
size_t reg_c{ 0 };

size_t p{ 0 };
std::vector<size_t> instructions;

std::vector<size_t> output;

size_t getCombo(size_t operand_code);
void execute(size_t opcode, size_t operand);


int main()
{
	shared::timer();

#if VISUAL_MODE == 0
	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list, true) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	unsigned int total{ 0 };

	int count{ 0 };
	for (auto& line : list)
	{
		std::vector<std::string> values;
		shared::getSpaceDelimitedValuesFromLine<std::string>(line, values);

		switch (count++)
		{
			case 0:
				reg_a = std::stoull(values[2]);
				break;
			case 1:
				reg_b = std::stoull(values[2]);
				break;
			case 2:
				reg_c = std::stoull(values[2]);
				break;
			case 3:
			{
				std::regex instruction_regex{ "(\\d+)" };
				auto instruction_begin{ std::sregex_iterator(values[1].begin(), values[1].end(), instruction_regex) };
				auto instruction_end{ std::sregex_iterator() };

				std::vector<size_t> nums;

				for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
				{
					auto instruction{ (*i).str() };
					instructions.push_back(std::stoull(instruction));
				}

				break;
			}
		}
	}

	for (; p < instructions.size() - 1; /* advanced in execute step */)
	{
		execute(instructions[p], instructions[p + 1]);
	}

	count = 0;
	for (auto item : output)
	{
		if (count++ != 0)
		{
			std::cout << ",";
		}

		std::cout << item;
	}

	std::cout << std::endl;
	
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
		// LoadFile() called before Init().
		void Init() override
		{
			// Sets SFML font / text data.
			m_font_size = 10;
			Challenge::Init();

			// Set how fast the animation should play.
			m_timestep = 0.01f;

			// Build initial data structure from input.
			for (auto& line : m_list)
			{
				// shared::getSpaceDelimitedValuesFromLine<int>(line, values);
			}
		}
				

		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt) override
		{
			return true;
		}


		// Drawing to the window. Called after Tick().
		void Render()
		{
			m_text.setString("Exaple text.");
			m_text.setFillColor(sf::Color(150, 150, 150, 255));
			m_text.setFillColor(sf::Color::Green);

			m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
			m_window->draw(m_text);
		}


		// std::vector<int> values;		
	};

	Puzzle puzzle{};

	// Uses puzzle size if true.
	// Use m_width/m_height as calculated in LoadFile().
	// Can override values in Init().
	// If false, uses config::screen_width/screen_height vaules.
	return vizit::run(puzzle, true);	

#endif
	shared::timer(true);
}


void execute(size_t opcode, size_t operand)
{
	bool advance_p{ true };

	switch (opcode)
	{
		case 0:
		{
			reg_a = reg_a / pow(2ull, getCombo(operand));
			break;
		}

		case 1:
		{
			reg_b = reg_b ^ operand;
			break;
		}

		case 2:
		{
			reg_b = getCombo(operand) % 8;
			break;
		}

		case 3:
		{
			if (reg_a != 0)
			{
				p = operand;
				advance_p = false;
			}
			break;
		}

		case 4:
		{
			reg_b = reg_b ^ reg_c;
			break;
		}

		case 5:
		{
			auto value = getCombo(operand) % 8;
			output.push_back(value);
			break;
		}

		case 6:
		{
			reg_b = reg_a / pow(2ull, getCombo(operand));
			break;
		}

		case 7:
		{
			reg_c = reg_a / pow(2ull, getCombo(operand));
			break;
		}
	}

	if (advance_p) p += 2;
}

size_t getCombo(size_t operand_code)
{
	switch (operand_code)
	{
		case 0:
		case 1:
		case 2:
		case 3:
			return operand_code;

		case 4:
			return reg_a;

		case 5:
			return reg_b;

		case 6:
			return reg_c;

		case 7:
			assert(false && "operand 7 should not be read.");
			return 0;

		default:
			assert(false && "unknown operand.");
			return 0;
	}
}