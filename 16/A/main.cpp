// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 2

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

#include <unordered_set>
#include <unordered_map>


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

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
#else
	// Visualize the puzzle using an extended class and SFML loop.
	
	// PUZZLE DATA //

	using Cell = shared::Cell<int>;

	class Puzzle : public vizit::Challenge
	{
	public:
		struct Path
		{
			Path() = default;

			Path(Cell c, Cell d, size_t s)
				: cell{ c }
				, direction{ d }
				, score{ s }
			{	}

			Cell cell;
			Cell direction;
			size_t score{ 0 };
			bool dead_end{ false };
			Cell parent;
		};

		// LoadFile() called before Init().
		void Init() override
		{
			// Sets SFML font / text data.
			m_font_size = 24;
			Challenge::Init();

			// Set how fast the animation should play.
			m_anim_delay = 0.00f;

			// Build initial data structure from input.
			int x{ 0 };
			int y{ 0 };

			for (auto& line : m_list)
			{
				x = 0;

				for (auto& letter : line)
				{
					if (letter == 'S')
					{
						start.x = x;
						start.y = y;
					}
					++x;
				}
				++y;
			}

			// First path.
			Path path{ start, shared::directions<int>[shared::Directions::RightIdx], 0 };
			auto path_hash{ shared::makeHash(path.cell.x, path.cell.y) };

			paths.insert(std::make_pair(path_hash, path));
			frontier.Put(path_hash, 0);
		}
				

		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt) override
		{
			bool did_advance{ false };

			auto prev{ frontier.Get() };
			auto prev_path{ paths[prev] };

			// Check neighbors.
			for (int i{ 0 }; i < 4; ++i)
			{
				auto direction{ shared::directions<int>[i] };

				// Skip going backwards.
				static Cell zero{ 0, 0 };
				if (prev_path.direction + direction == zero) continue;

				// Skip walls.
				Cell new_pos{ prev_path.cell + direction };
				if (m_list[new_pos.y][new_pos.x] == '#') continue;


				if (visited.find(new_pos) != visited.end()) continue;

				// Use new direction & position.
				size_t gained_points{ 0 };
				Path neighbor{ new_pos, direction, prev_path.score };

				// Determine if next step would require a turn.
				if (m_list[neighbor.cell.y][neighbor.cell.x] == '.')
				{
					gained_points += 1;

					// Turn!
					if (prev_path.direction != direction)
					{
						gained_points += 1000;
					}

					neighbor.score += gained_points;

					// If already visited, see if new cost is better.
					if (visited.find(new_pos) != visited.end())
					{
						//if (neighbor.score < paths[shared::makeHash(new_pos.x, new_pos.y)].score)
					}

					// Add to queue.
					neighbor.parent = prev_path.cell;
					auto neighbor_hash{ shared::makeHash(neighbor.cell.x, neighbor.cell.y) };
					paths.insert(std::make_pair(neighbor_hash, neighbor));
					frontier.Put(neighbor_hash, neighbor.score);

					did_advance = true;
				}

				// Finish!
				else
				{
					neighbor.parent = prev_path.cell;
					auto neighbor_hash{ shared::makeHash(neighbor.cell.x, neighbor.cell.y) };
					//paths.insert(std::make_pair(neighbor_hash, neighbor));

					visited.insert(prev_path.cell);

					end = neighbor;

					m_total = end.score + 1;
					return true;
				}
			}
			
			visited.insert(prev_path.cell);

			if (!did_advance)
			{
				dead_ends.insert(prev_path.cell);
			}

			return false;
		}


		// Drawing to the window. Called after Tick().
		void Render(bool is_done)
		{
			// Map.
			int x{ 0 };
			int y{ 0 };

			for (auto& line : m_list)
			{
				x = 0;
				for (auto& letter : line)
				{
					if (letter == '#')
					{
						m_text.setString("#");
						m_text.setFillColor(sf::Color::White);
						m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
						m_window->draw(m_text);
					}
					++x;
				}
				++y;
			}

			// Queued cells.
			for (auto it{ paths.begin() }; it != paths.end(); ++it)
			{
				auto cell{ (*it).second.cell };

				m_text.setString("X");
				m_text.setFillColor(sf::Color::Yellow);
				m_text.setPosition({ static_cast<float>(cell.x * m_font_size), static_cast<float>(cell.y * m_font_size) });
				m_window->draw(m_text);
			}

			// Visited cells.
			for (auto it{ visited.begin() }; it != visited.end(); ++it)
			{
				auto cell{ *it };

				m_text.setString("X");
				m_text.setFillColor(sf::Color(150, 150, 150, 255));
				m_text.setPosition({ static_cast<float>(cell.x * m_font_size), static_cast<float>(cell.y * m_font_size) });
				m_window->draw(m_text);
			}

			// Dead ends
			for (auto it{ dead_ends.begin() }; it != dead_ends.end(); ++it)
			{
				auto cell{ *it };

				m_text.setString("X");
				m_text.setFillColor(sf::Color::Red);
				m_text.setPosition({ static_cast<float>(cell.x * m_font_size), static_cast<float>(cell.y * m_font_size) });
				m_window->draw(m_text);
			}

			// Winning path.
			if (is_done)
			{
				auto path{ end };

				while (path.cell != start)
				{
					m_text.setString("O");
					m_text.setFillColor(sf::Color::Green);
					m_text.setPosition({ static_cast<float>(path.cell.x * m_font_size), static_cast<float>(path.cell.y * m_font_size) });
					m_window->draw(m_text);

					path = paths[shared::makeHash(path.parent.x, path.parent.y)];
				}

				// Final (start) cell.
				m_text.setString("O");
				m_text.setFillColor(sf::Color::Green);
				m_text.setPosition({ static_cast<float>(start.x * m_font_size), static_cast<float>(start.y * m_font_size) });
				m_window->draw(m_text);
			}
		}
				
		// Track paths.
		std::unordered_map<unsigned int, Path> paths;

		// Paths to visit.
		container::PairedPriorityQueue<size_t, unsigned int> frontier;

		// Cells visited.
		std::unordered_set<Cell, Cell::HashFunction> visited;

		// Render help.
		std::unordered_set<Cell, Cell::HashFunction> dead_ends;

		// Important cells.
		Cell start;
		Path end;
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