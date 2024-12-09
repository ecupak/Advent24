// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 0 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE false


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