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

		void Init() override
		{
			m_width = width;
			m_height = height;

			// Sets SFML font / text data.
			m_font_size = 8;
			Challenge::Init();

			// Set how fast the animation should play.
			m_anim_delay = 0.5f;

			for (size_t i{ 0 }; i < m_list.size(); ++i)
			{
				Info& info{ robots[i] };

				auto& line{ m_list[i] };

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

				info.start.x = nums[0];
				info.start.y = nums[1];

				info.velocity.x = nums[2];
				info.velocity.y = nums[3];

				info.robot.x = info.start.x;
				info.robot.y = info.start.y;

				// Run all iterations.
				size_t max_iterations{ 101 * 103 };
				history.resize(max_iterations);

				for (size_t s{ 0 }; s < max_iterations + 1; ++s)
				{
					std::unordered_map<bigint, bigint> quadrants;

					for (size_t i{ 0 }; i < m_list.size(); ++i)
					{
						Info& info{ robots[i] };

						// Calculate future position.

						info.robot.x = getPosition(s, width, info.start.x, info.velocity.x);
						info.robot.y = getPosition(s, height, info.start.y, info.velocity.y);

						// Find quadrant.					

						if (info.robot.x > mid_point_x && info.robot.y > mid_point_y)
							++quadrants[0];
						else if (info.robot.x < mid_point_x && info.robot.y > mid_point_y)
							++quadrants[1];
						else if (info.robot.x < mid_point_x && info.robot.y < mid_point_y)
							++quadrants[2];
						else
							++quadrants[3];
					}

					history[s] = std::make_pair(quadrants[0] * quadrants[1] * quadrants[2] * quadrants[3], s);
				}

				// Sort history.
				std::sort(history.begin(), history.end());
			}
		}


		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt) override
		{
			m_total = ++seconds_ahead;
			cells.clear();

			//for (size_t h{ 0 }; h < 100; ++h)
			{
				bigint s{ history[seconds_ahead].second };

				for (size_t i{ 0 }; i < m_list.size(); ++i)
				{
					Info& info{ robots[i] };

					// Calculate future position.

					info.robot.x = getPosition(s, width, info.start.x, info.velocity.x);
					info.robot.y = getPosition(s, height, info.start.y, info.velocity.y);

					++cells[info.robot];
				}

				save_screenshot = true;

				if (seconds_ahead >= 100)
					return true;

				return false;
			}
		}


		// Drawing to the window. Called after Tick().
		void Render()
		{
			shared::Cell<bigint> filler;

			for (int y{ 0 }; y < height; ++y)
			{
				for (int x{ 0 }; x < width; ++x)
				{
					filler.x = x;
					filler.y = y;
					
					auto value{ cells[filler] };

					m_text.setString(std::to_string(value));

					if (value == 0)
						m_text.setFillColor(value > 0 ? sf::Color::White : sf::Color(170, 170, 170, 255));					
					else
						m_text.setFillColor(sf::Color::White);

					m_text.setPosition({ 1.0f * x * m_font_size, 1.0f * y * m_font_size });
					m_window->draw(m_text);
				}
			}

			// Save screenshot.
			if (save_screenshot)
			{
				save_screenshot = false;

				sf::Texture texture;
				texture.create(m_window->getSize().x, m_window->getSize().y);
				texture.update(*m_window);

				static size_t n_zero{ 4 };
				std::string num{ std::to_string(shared::numDigits<bigint>(history[seconds_ahead].second)) };
				std::string padded_num{ std::string(n_zero - std::min(n_zero, num.length()), '0') + std::to_string(history[seconds_ahead].second) };
				std::string filename{ "../search/robots_" + padded_num + ".png" };
				texture.copyToImage().saveToFile(filename);
			}
		}

		bigint seconds{ 100 };
		bigint seconds_ahead{ -1 };

#if USE_INPUT_FILE
		bigint width{ 101 };
		bigint height{ 103 };
#else
		bigint width{ 11 };
		bigint height{ 7 };
#endif
		bigint mid_point_x{ (width - 1) / 2 };
		bigint mid_point_y{ (height - 1) / 2 };

		std::vector<std::pair<bigint, bigint>> history;
		
		struct Info
		{
			shared::Cell<bigint> robot;
			shared::Cell<bigint> start;
			shared::Cell<bigint> velocity;			
		};

		std::unordered_map<size_t, Info> robots;
		std::unordered_map<shared::Cell<bigint>, bigint, shared::Cell<bigint>::HashFunction> cells;
		bool save_screenshot{ false };
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