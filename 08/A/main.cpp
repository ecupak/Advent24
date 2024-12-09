// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 0 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

#include <unordered_map>
#include <unordered_set>


bool IsInBounds(size_t width, size_t height, const shared::Cell& cell)
{
	return cell.x >= 0 && cell.x < width && cell.y >= 0 && cell.y < height;
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

	unsigned int total{ 0 };

	std::unordered_map<char, std::vector<shared::Cell>> antenna_locations;

	// When a non-'.' is found, add location to map of similar dots.
	size_t y{ 0 };
	size_t x{ 0 };
	
	for (auto& line : list)
	{
		if (line.empty()) continue;

		x = 0;

		for (auto letter : line)
		{
			if (letter != '.')
			{
				antenna_locations[letter].emplace_back(x, y);
			}

			++x;
		}

		++y;
	}

	size_t width{ x };
	size_t height{ y };

	// Check each type of frequency and create antinodes.
	std::unordered_set<shared::Cell, shared::Cell::HashFunction> anti_nodes;

	for (auto& [antenna_id, locations] : antenna_locations)
	{
		for (int i{ 0 }; i < locations.size(); ++i)
		{
			shared::Cell& source{ locations[i] };

			for (int k{ 0 }; k < locations.size(); ++k)
			{
				// Don't compare against self.
				if (i == k) continue;

				shared::Cell anti_node_offset{ source - locations[k] };

				anti_nodes.insert(source + anti_node_offset);
			}
		}
	}

	// Count only the ones on the map.
	for (auto& cell : anti_nodes)
	{
		if (IsInBounds(width, height, cell))
		{
			total += 1;
		}
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