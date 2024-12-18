// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 1 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>


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

	// std::vector<int> values;

	for (auto& line : list)
	{
		// shared::getSpaceDelimitedValuesFromLine<int>(line, values);
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
		// LoadFile() called before Init().
		void Init() override
		{
			// Sets SFML font / text data.
			m_font_size = 24;
			Challenge::Init();

			// Set how fast the animation should play.
			m_anim_delay = 0.00f;

			// Build initial data structure from input.
			int y{ 0 };
			size_t x{ 0 };
			m_height = 0;

			for (auto& line : m_list)
			{
				if (line[0] == '#')
				{
					x = line.size();

					// Get robot position.
					if (auto pos{ line.find('@') }; pos != std::string::npos)
					{
						robot.x = static_cast<int>(pos);
						robot.y = y;
						line[pos] = '.';
					}

					warehouse += line;

					++m_height;
				}
				else
				{
					inputs += line;
				}

				++y;
			}

			m_width = static_cast<int>(x);			
			++m_height;

			width = m_width;
			height = m_height - 1;
		}
				

		bool TryPush(shared::Cell<int> destination, shared::Cell<int>& direction)
		{
			auto original_destination{ destination };

			// Look for '.' along path.
			while (true)
			{
				destination += direction;

				auto letter{ warehouse[destination.x + destination.y * width] };

				switch (letter)
				{
					// Move box
					case '.':
						warehouse[original_destination.x + original_destination.y * width] = '.';
						warehouse[destination.x + destination.y * width] = 'O';						
						return true;

					// Box stuck
					case '#':
						return false;
				}
			}

			return false;
		}


		bool MoveRobot(int dir)
		{
#if VISUAL_MODE == 1
			shadows.write(robot);
#endif
			auto& direction{ shared::directions<int>[dir] };

			auto destination{ robot + direction };

			auto letter{ warehouse[destination.x + destination.y * width] };

			switch (letter)
			{
				// Move
				case '.':
					robot = destination;
					return true;

				// Stay
				case '#':
					return false;

				// Push
				default: // '0'
					if (TryPush(destination, direction))
					{
						robot = destination;
						return true;
					}
					else
					{
						return false;
					}
			}
		}


		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt) override
		{
			auto input{ inputs[++input_idx] };

			switch (input)
			{
				case '<':
					input_outcome = MoveRobot(shared::Directions::LeftIdx);
					break;
				case '>':
					input_outcome = MoveRobot(shared::Directions::RightIdx);
					break;
				case '^':
					input_outcome = MoveRobot(shared::Directions::UpIdx);
					break;
				case 'v':
					input_outcome = MoveRobot(shared::Directions::DownIdx);
					break;
				default:
					input_outcome = false;
					break;
			}

			if (input_idx + 1 >= inputs.size())
			{
				// Calculate score.
				for (unsigned int y{ 0 }; y < height; ++y)
				{
					auto y_offset{ y * width };

					for (unsigned int x{ 0 }; x < width; ++x)
					{
						if (warehouse[x + y_offset] == 'O')
						{
							m_total += x + y * 100;
						}
					}
				}

				return true;
			}
			else
			{
				return false;
			}
		}


		// Drawing to the window. Called after Tick().
		void Render()
		{		
			// Past positions.
			m_text.setString('@');
			
			unsigned int alpha{ 160 };
			for (const auto& shadow : shadows)
			{
				m_text.setFillColor(sf::Color(0, 255, 255, alpha));
				m_text.setPosition({ static_cast<float>(shadow.x * m_font_size), static_cast<float>(shadow.y * m_font_size) });
				m_window->draw(m_text);

				alpha -= 15;
			}

			// Map
			for (size_t y{ 0 }; y < height; ++y)
			{
				auto y_offset{ y * width };

				for (size_t x{ 0 }; x < width; ++x)
				{
					auto letter{ warehouse[x + y_offset] };
					m_text.setString(letter);

					switch (letter)
					{
						case '#':
							m_text.setFillColor(sf::Color::White);
							break;
						case 'O':
							m_text.setFillColor(sf::Color::Yellow);
							break;
						default:
							m_text.setFillColor(sf::Color(150, 150, 150, 255));
							break;
					}
					
					m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
					m_window->draw(m_text);
				}
			}

			// Robot
			m_text.setString('@');
			m_text.setFillColor(sf::Color::Cyan);
			m_text.setPosition({ static_cast<float>(robot.x * m_font_size), static_cast<float>(robot.y * m_font_size) });
			m_window->draw(m_text);

			// Input line
			static size_t mid{ (width / 2) - 1 };
			static size_t rem{ width - 1 - mid };

			// .. Show consumed inputs.
			m_text.setFillColor(sf::Color(150, 150, 150, 255));
			for (size_t x{ 0 }; x < mid; ++x)
			{
				int old_idx{ static_cast<int>(input_idx - mid + x) };
				if (old_idx >= 0)				
				{
					m_text.setString(inputs[old_idx]);
					m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(height * m_font_size) });
					m_window->draw(m_text);
				}
			}

			// .. Show current input.
			if (input_idx >= 0)
			{
				m_text.setString(inputs[input_idx]);
				m_text.setFillColor(input_outcome ? sf::Color::Green : sf::Color::Red);
				m_text.setPosition({ static_cast<float>(mid * m_font_size), static_cast<float>(height * m_font_size) });
				m_window->draw(m_text);
			}

			// .. Show upcoming inputs.
			m_text.setFillColor(sf::Color(180, 180, 180, 255));
			for (size_t x{ 0 }; x < rem; ++x)
			{
				int next_idx{ static_cast<int>(input_idx + x + 1) };
				if (next_idx < inputs.size())
				{
					m_text.setString(inputs[next_idx]);
					m_text.setPosition({ static_cast<float>((mid + x + 1) * m_font_size), static_cast<float>(height * m_font_size) });
					m_window->draw(m_text);
				}
			}
		}

		int input_idx{ -1 };
		bool input_outcome{ false };

		std::string warehouse;
		std::string inputs;
		shared::Cell<int> robot;
		container::RingBuffer<shared::Cell<int>> shadows{ 10 };

		size_t width{ 0 };
		size_t height{ 0 };
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