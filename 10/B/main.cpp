// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 1 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

#include <unordered_set>
#include <queue>


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

	int x{ 0 };
	int y{ 0 };

	for (auto& line : list)
	{
		x = 0;

		// Find each 0 and spread from there.
		for (auto letter : line)
		{
			if (letter == '0')
			{
				total += FindTrails(x, y, list);
			}
			++x;
		}

		++y;
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
			m_font_size = 24;
			Challenge::Init();
		}

		// Drawing to the window.
		void Render()
		{
			int rx{ 0 };
			int ry{ 0 };

			// Base map.
			for (auto& line : m_list)
			{
				rx = 0;
				for (auto letter : line)
				{
					m_text.setString(letter);
					m_text.setFillColor(sf::Color(170, 170, 170, 255));
					m_text.setPosition({ static_cast<float>(rx * m_font_size), static_cast<float>(ry * m_font_size) });
					m_window->draw(m_text);

					++rx;
				}

				++ry;
			}

			// Visited cells.
			for (auto cell : visited)
			{
				m_text.setString('X');
				m_text.setFillColor(sf::Color::Yellow);
				m_text.setPosition({ static_cast<float>(cell.x * m_font_size), static_cast<float>(cell.y * m_font_size) });
				m_window->draw(m_text);
			}
		}


		size_t FindTrails(int start_x, int start_y, int level_to_find, std::vector<std::string>& list)
		{
			size_t total{ 0 };

			shared::Cell start{ start_x, start_y };

			// Check all directions around current cell / pos.
			for (int dir{ 0 }; dir < 4; ++dir)
			{
				auto nearby_cell{ start + shared::directions[dir] };

				try
				{
					// If next level / gradual slope.
					auto nearby_cell_level{ list.at(nearby_cell.y).at(nearby_cell.x) - 48 };

					if (nearby_cell_level == level_to_find)
					{
						if (level_to_find == 9)
						{
							total += 1;
							visited.insert(nearby_cell);
						}
						else
						{
							total += FindTrails(nearby_cell.x, nearby_cell.y, level_to_find + 1, list);
						}
					}
				}
				catch (...) { }
			}

			visited.insert(start);

			return total;
		}


		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt, bool can_advance) override
		{
			static float elapsed_time{ 0.0f };
			static float timestep{ 0.01f };

			elapsed_time += dt;

			if (can_advance && elapsed_time >= timestep)
			{
				try
				{
					while (m_list.at(y).at(x) != '0')
					{
						if (++x >= m_width)
						{
							x = 0;
							++y;
						}
					}
				}
				catch (...)
				{
					return true;
				}

				visited.clear();
				m_total += FindTrails(x, y, 1, m_list);

				if (++x >= m_width)
				{
					x = 0;
					++y;
				}
			}

			Render();
			return false;
		}

		std::unordered_set<shared::Cell, shared::Cell::HashFunction> visited;
		int x{ 0 };
		int y{ 0 };
	};

	Puzzle puzzle{};

	// Can optionally pass the screen width and height to use.
	// Default will use the config settings.
	// Challenge class contains width and height of input, based on first line.
	return vizit::run(puzzle, true);
	//return vizit::run(puzzle, 400, 600);

#endif
	shared::timer(true);
}
