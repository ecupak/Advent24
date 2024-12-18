// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 1 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

// Regex.
#include <iostream>
#include <iterator>
#include <regex>

#include <unordered_map>
#include <unordered_set>


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

	std::vector<bool> grid(71 * 71, true);

	std::vector<int> point;

	for (auto& line : list)
	{
		shared::getSpaceDelimitedValuesFromLine<int>(line, point);

		grid[point[0] + point[1] * 71] = false;
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
		using Cell = shared::Cell<int>;

		// LoadFile() called before Init().
		void Init() override
		{
			// Sets SFML font / text data.
			m_font_size = 16;
			Challenge::Init();

			// Set how fast the animation should play.
			m_anim_delay = 0.01f;

			grid = std::vector<bool>(71 * 71, true);

			std::vector<int> point;

			m_width = 71;
			m_height = 71;

			int count{ 0 };
			for (auto& line : m_list)
			{
				std::regex instruction_regex{ "(\\d+)" };
				auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
				auto instruction_end{ std::sregex_iterator() };

				std::vector<int> nums;

				for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
				{
					nums.push_back(std::stoi((*i).str()));
				}

				grid[nums[0] + nums[1] * m_width] = false;

				if (count++ == 1024) break;
			}

			Node first_node;
			first_node.cost = 0;

			size_t hash{ Cell::HashFunction()(first_node.cell) };
			
			frontier.Put(hash, 0);
			nodes[hash].cost = 0;
		}
				
		struct Node
		{
			Node() = default;

			Node(Cell c, int t)
				: cell{ c }
				, cost{ t }
			{	}

			Cell cell;
			int cost{ -1 };
			size_t parent_hash{ 0 };
		};

		container::PairedPriorityQueue<size_t, size_t> frontier;
		//std::queue<size_t> frontier;

		std::unordered_map<size_t, Node> nodes;
		std::unordered_set<Cell, Cell::HashFunction> visited;
		std::unordered_set<Cell, Cell::HashFunction> path;

		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt) override
		{			
			auto& parent_node{ nodes[frontier.Get()] };
			auto& parent_cell{ parent_node.cell };
			
			// If already visited, skip.
			if (visited.find(parent_cell) != visited.end()) return false;

			for (int i{ 0 }; i < 4; ++i)
			{
				// Get neighbor cell.
				auto direction{ shared::directions<int>[i] };
				auto n_cell{ parent_cell + direction };

				// If out of bounds, skip.
				if (n_cell.x < 0 || n_cell.x >= m_width || n_cell.y < 0 || n_cell.y >= m_height) continue;
					
				// If a wall, skip.
				if (!grid[n_cell.x + n_cell.y * m_width]) continue;

				size_t hash{ Cell::HashFunction()(n_cell) };
				auto& neighbor_node{ nodes[hash] };

				// If better cost found or never visited before, set cost.
				if (neighbor_node.cost == -1 || parent_node.cost + 1 < neighbor_node.cost)
				{
					neighbor_node.cell = n_cell;
					neighbor_node.cost = parent_node.cost + 1;
					neighbor_node.parent_hash = Cell::HashFunction()(parent_cell);
				}

				// If on exit tile, escape.
				if (n_cell.x == m_width - 1 && n_cell.y == m_height - 1)
				{
					m_total = neighbor_node.cost;
					visited.insert(parent_cell);
					visited.insert(n_cell);

					// Build path along parent cells.
					path.insert(n_cell);
					path.insert(parent_cell);
					Node next_node{ parent_node };
					while (next_node.cost != 0)
					{
						path.insert(next_node.cell);
						next_node = nodes[next_node.parent_hash];
					}
					path.insert(next_node.cell);

					return true;
				}

				// Add to frontier queue.
				if (visited.find(n_cell) == visited.end())
				{
					size_t priority_value{ static_cast<size_t>((m_width - 1u - n_cell.x) + (m_height - 1u - n_cell.y)) };
					frontier.Put(hash, priority_value);
				}
			}
			
			visited.insert(parent_cell);
			return frontier.IsEmpty();
		}


		// Drawing to the window. Called after Tick().
		void Render(bool is_done) override
		{
			int x{ 0 };
			int y{ 0 };

			for (int y{ 0 }; y < m_height; ++y)
			{
				for (int x{ 0 }; x < m_width; ++x)
				{
					if (grid[x + y * m_width])
					{
						m_text.setString(".");
						m_text.setFillColor(sf::Color(150, 150, 150, 255));
					}
					else
					{
						m_text.setString("#");
						m_text.setFillColor(sf::Color::White);
					}

					m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
					m_window->draw(m_text);
				}
			}

			for (auto& cell : visited)
			{
				m_text.setString("O");
				m_text.setFillColor(sf::Color::Yellow);
				m_text.setPosition({ static_cast<float>(cell.x * m_font_size), static_cast<float>(cell.y * m_font_size) });
				m_window->draw(m_text);
			}

			if (is_done)
			{
				for (auto& cell : path)
				{
					m_text.setString("+");
					m_text.setFillColor(sf::Color::Green);
					m_text.setPosition({ static_cast<float>(cell.x * m_font_size), static_cast<float>(cell.y * m_font_size) });
					m_window->draw(m_text);
				}
			}
		}

		std::vector<bool> grid;
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