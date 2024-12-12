// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 0 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

#include <unordered_map>



using Memo = std::unordered_map<size_t, std::unordered_map<size_t, size_t>>;


bool splitRock(size_t value, std::pair<size_t, size_t>& split)
{
	if (auto count{ shared::numDigits(value) }; (count % 2) == 0)
	{
		std::string str{ std::to_string(value) };

		auto first_str{ str.substr(0, count / 2) };
		auto first{ static_cast<size_t>(std::stoull(first_str)) };

		auto second_str{ str.substr(count / 2) };
		auto second{ static_cast<size_t>(std::stoull(second_str)) };

		split = std::make_pair(first, second);

		return true;
	}

	return false;
}


size_t blink(size_t value, size_t steps, Memo& memo)
{
	// Every stone returns a count of 1 after last step is completed.
	if (steps == 0) return 1;

	// If number was seen at this stage before, return its pre-calcualted result.
	try
	{
		auto known_result{ memo.at(steps).at(value) };

		return known_result;
	}

	// Otherwise, calculate it now.
	catch (...)
	{
		size_t result{ 0 };

		std::pair<size_t, size_t> split;

		if (value == 0)
			result = blink(1, steps - 1, memo);
		else if (splitRock(value, split))
			result = blink(split.first, steps - 1, memo) + blink(split.second, steps - 1, memo);
		else
			result = blink(value * 2024, steps - 1, memo);

		// Store result for this value at this iteration (blink step).
		memo[steps][value] = result;

		return result;
	}
}


int main()
{
	shared::timer();

#if VISUAL_MODE == 0
	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list, true) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

#if USE_INPUT_FILE
	size_t blinks{ 75 };
#else
	size_t blinks{ 6 };
#endif

	unsigned int total{ 0 };

	std::vector<size_t> values;

	for (auto& line : list)
	{
		shared::getSpaceDelimitedValuesFromLine<size_t>(line, values);
	}

	Memo memo;
	for (auto& value : values)
	{
		total += blink(value, blinks, memo);
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