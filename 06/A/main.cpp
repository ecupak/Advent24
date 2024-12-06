#define SKIP_VISUALIZE true

#include "shared.h"

#include <iostream>
#include <vector>

#include <unordered_set>


static size_t lookup2D(shared::Cell& cell, size_t height) { return cell.x + cell.y * height; }


class Walker : public vizit::Challenge
{
public:		
	void Init() override
	{
		m_font_size = 10;
		Challenge::Init();

		int x{ 0 };
		int y{ 0 };

		for (const auto& line : m_list)
		{
			x = 0;

			// Build map
			for (auto& letter : line)
			{
				// True if empty.
				m_grid.push_back(letter == '#' ? 0 : 1);

				if (letter == '^')
				{
					m_cell.x = x;
					m_cell.y = y;
					m_next_cell = m_cell;
				}
				++x;
			}
			++y;
		}

		m_dir = shared::Directions::UpIdx;
		m_direction = shared::directions[m_dir];
	}


	void RenderGrid()
	{
		for (size_t y{ 0 }; y < m_height; ++y)
		{
			for (size_t x{ 0 }; x < m_width; ++x)
			{
				switch (m_grid[x + y * m_height])
				{
					case 0:
						m_text.setString("#");
						m_text.setFillColor(sf::Color::Yellow);
						break;
					case 1:
						m_text.setString(".");
						m_text.setFillColor(sf::Color::White);
						break;
					case 2:
						m_text.setString("X");
						m_text.setFillColor(sf::Color(150, 150, 150, 255));
						break;
					default:
						m_text.setString("?");
						m_text.setFillColor(sf::Color::White);
						break;
				}

				if (m_cell.x == x && m_cell.y == y)
					m_text.setFillColor(sf::Color::White);
				else if (m_next_cell.x == x && m_next_cell.y == y)
				{
					m_text.setString("0");
					m_text.setFillColor(sf::Color::Green);
				}

				m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
				m_window->draw(m_text);
			}
		}
	}


	bool InBounds(shared::Cell& cell)
	{
		return cell.x >= 0 && cell.x < m_width && cell.y >= 0 && cell.y < m_height;
	}


	bool Tick(float, bool can_advance) override
	{		
		bool is_on_map{ true };

		if (can_advance)
		{	
			// Move cell.
			m_cell = m_next_cell;

			// Mark map.
			if (m_grid[lookup2D(m_cell, m_height)] == 1)
			{
				m_total += 1;
				m_grid[lookup2D(m_cell, m_height)] = 2;
			}

			// Check / Update direction.			
			while (true)
			{
				m_next_cell = m_cell + m_direction;
				
				// Done (off map).
				if (!InBounds(m_next_cell)) return true;

				// Change direction if next cell is an obstacle.
				if (m_grid[lookup2D(m_next_cell, m_height)] == 0)
				{
					m_dir = ((m_dir + 1) % shared::Directions::Count);
					m_direction = shared::directions[m_dir];
				}
				else
				{
					break;
				}
			};
		}

		RenderGrid();
		
		// Still on map.
		return false;
	}
		
	shared::Cell m_cell{ };
	shared::Cell m_next_cell{ };
	std::vector<int> m_grid;

	int m_dir{ 0 };
	shared::Cell m_direction{ };
};

unsigned int makeHash(int x, int y)
{
	return x + y * 10000;
}

int main()
{
	//Walker walker{};
	//return vizit::run(walker, true);

	shared::timer();

	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	unsigned int total{ 0 };

	std::unordered_set<unsigned int> obstacles;
	std::unordered_set<unsigned int> spaces;
	std::unordered_set<unsigned int> visited;

	// Fill sets.
	int y{ 0 };
	int x{ 0 };
	shared::Cell cell{ };

	for (const auto& line : list)
	{
		x = 0;

		for (auto letter : line)
		{
			switch (letter)
			{
				case '#':
					obstacles.insert(makeHash(x, y));
					break;
				case '.':
					spaces.insert(makeHash( x, y ));
					break;
				case '^':
					spaces.insert(makeHash(x, y));
					cell = shared::Cell{ x, y };
					break;
			}
			++x;
		}
		++y;
	}

	// Explore map.
	int dir{ shared::Directions::UpIdx };
	shared::Cell direction{ shared::directions[dir] };

	// Check / Update direction.
	unsigned int cell_hash{ makeHash(cell.x, cell.y) };
	unsigned int next_cell_hash{ 0 };
	while (true)
	{
		// Mark current cell as visited.
		visited.insert(cell_hash);

		shared::Cell next_cell{ cell + direction };
		next_cell_hash = makeHash(next_cell.x, next_cell.y);

		// See if empty cell.
		if (auto iter{ spaces.find(next_cell_hash) }; iter != spaces.end())
		{
			// Move to next cell.
			cell = next_cell;
			cell_hash = makeHash(cell.x, cell.y);
		}

		// See if obstacle.
		else if (auto iter{ obstacles.find(next_cell_hash) }; iter != obstacles.end())
		{
			// Rotate.
			switch (dir)
			{
				case 0: dir = 1; break;
				case 1: dir = 2; break;
				case 2: dir = 3; break;
				case 3: dir = 0; break;
			}

			direction = shared::directions[dir];
		}

		// Neither - off map!
		else
		{
			break;
		}
	};

	total = visited.size();

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);
}