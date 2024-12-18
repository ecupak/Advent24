// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 1

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <unordered_set>

// Regex.
#include <iostream>
#include <iterator>
#include <regex>

using bigint = long long;


int main()
{
	shared::timer();

#if VISUAL_MODE == 0
	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list, true) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	bigint seconds{ 100 };
	bigint width{ 11 };
	bigint height{ 7 };

	bigint total{ 0 };

	shared::Cell<bigint> start;
	shared::Cell<bigint> velocity;

	std::unordered_map<unsigned int, unsigned int> quadrants;

	for (auto& line : list)
	{
		// Get starting position and velocity.

		std::string regex_str = "(-*\\d+)";

		std::regex instruction_regex{ regex_str };
		auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
		auto instruction_end{ std::sregex_iterator() };

		std::vector<bigint> nums;

		for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
		{
			auto str{ (*i).str() };

			nums.push_back(std::stoll((*i).str()));
		}

		// Calcualte future position.

		bigint x{ getPosition(seconds, width, nums[0], nums[2]) };
		bigint y{ getPosition(seconds, height, nums[1], nums[3]) };

		// Find quadrant.

		static bigint mid_point_x{ (width - 1) / 2 };
		static bigint mid_point_y{ (height - 1) / 2 };

		if (x > mid_point_x && y > mid_point_y)
			++quadrants[0];
		else if (x < mid_point_x && y > mid_point_y)
			++quadrants[1];
		else if (x < mid_point_x && y < mid_point_y)
			++quadrants[2];
		else if (x > mid_point_x && y < mid_point_y)
			++quadrants[3];
		else
			++quadrants[4];
	}

	total = quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];

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
			m_width = width;
			m_height = height;

			// Sets SFML font / text data.
			m_font_size = 12;
			Challenge::Init();

			// Set how fast the animation should play.
			m_anim_delay = 0.5f;
		}
				

		bigint getPosition(bigint steps, bigint size, bigint start, bigint velocity)
		{
			bool is_negative{ velocity < 0 };

			if (is_negative)
			{
				velocity *= -1ll;
			}

			bigint final{ 0 };

			final = velocity * steps;
			final = (final % size);
			final = is_negative ? size - final : final;
			final = (final + start) % size;

			return final;
		}


		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt) override
		{
			auto& line{ m_list[line_idx] };
			
			// Get starting position and velocity.

			std::string regex_str = "(-*\\d+)";

			std::regex instruction_regex{ regex_str };
			auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
			auto instruction_end{ std::sregex_iterator() };

			std::vector<bigint> nums;

			for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
			{
				auto str{ (*i).str() };

				nums.push_back(std::stoll((*i).str()));
			}

			// Calcualte future position.

			bigint x{ getPosition(seconds, width, nums[0], nums[2]) };
			bigint y{ getPosition(seconds, height, nums[1], nums[3]) };

			new_cell = shared::Cell{ x, y };
			++cells[new_cell];

			// Find quadrant.					

			if (x > mid_point_x && y > mid_point_y)
				++quadrants[0];
			else if (x < mid_point_x && y > mid_point_y)
				++quadrants[1];
			else if (x < mid_point_x && y < mid_point_y)
				++quadrants[2];
			else if (x > mid_point_x && y < mid_point_y)
				++quadrants[3];
			else
				++quadrants[4];

			if (++line_idx >= m_list.size())
			{
				m_total = quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3];
				return true;
			}

			return false;
		}


		// Drawing to the window. Called after Tick().
		void Render()
		{

			// Split the field into quadrants.

			shared::Cell<bigint> filler;

			for (int y{ 0 }; y < height; ++y)
			{
				for (int x{ 0 }; x < width; ++x)
				{
					filler.x = x;
					filler.y = y;

					if (x == mid_point_x || y == mid_point_y)
					{
						m_text.setString("X");

						if (filler == new_cell)
							m_text.setFillColor(sf::Color::Red);
						else
							m_text.setFillColor(sf::Color::Yellow);
					}
					else
					{
						auto value{ cells[filler] };

						m_text.setString(std::to_string(value));

						if (filler == new_cell)
							m_text.setFillColor(sf::Color::Green);
						else
							m_text.setFillColor(value > 0 ? sf::Color::White : sf::Color(170, 170, 170, 255));
					}
					
					m_text.setPosition({ 1.0f * x * m_font_size, 1.0f * y * m_font_size });
					m_window->draw(m_text);
				}
			}			
		}

		size_t line_idx{ 0 };

		bigint seconds{ 100 };
		
#if USE_INPUT_FILE
		bigint width{ 101 };
		bigint height{ 103 };
#else
		bigint width{ 11 };
		bigint height{ 7 };
#endif
		bigint mid_point_x{ (width - 1) / 2 };
		bigint mid_point_y{ (height - 1) / 2 };

		std::unordered_map<unsigned int, unsigned int> quadrants;
		std::unordered_map<shared::Cell<bigint>, bigint, shared::Cell<bigint>::HashFunction> cells;
		shared::Cell<bigint> new_cell{ -1, -1 };
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