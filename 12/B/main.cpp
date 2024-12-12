// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 2

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

#include <unordered_set>
#include <unordered_map>
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
			m_font_size = 8;
			Challenge::Init();
		}

		// Drawing to the window.
		void Render()
		{
#if VISUAL_MODE == 1
			int x{ 0 };
			int y{ 0 };

			for (auto& line : m_list)
			{
				x = 0;

				for (auto& letter : line)
				{
					shared::Cell cell{ x, y };

					m_text.setString(letter);

					if (m_visited.find(cell) != m_visited.end())
						m_text.setFillColor(sf::Color::Green);
					else if (letter == '.')
						m_text.setFillColor(sf::Color(150, 150, 150, 255));
					else
						m_text.setFillColor(sf::Color::White);

					m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
					m_window->draw(m_text);
					++x;
				}
				++y;
			}
#endif
		}


		int getSides()
		{
			struct Line
			{
				Line() = default;

				Line(shared::Cell a, shared::Cell b)
					: begin{ a }
					, end{ b }
				{	}

				Line(int begin_x, int begin_y, int end_x, int end_y)
					: begin{ begin_x, begin_y }
					, end{ end_x, end_y }
				{	}

				shared::Cell begin;
				shared::Cell end;

				bool operator==(const Line& other) const
				{
					return (begin == other.begin && end == other.end);
				}

				std::string toString() const
				{
					return "(" + begin.toString() + ")" + ", " + "(" + end.toString() + ")";
				}

				struct Equality
				{
					bool operator()(const Line& lhs, const Line& rhs) const
					{
						return ((lhs.begin == rhs.begin && lhs.end == rhs.end)
								|| (lhs.begin == rhs.end && lhs.end == rhs.begin));
					}
				};

				struct HashFunction
				{
					size_t operator()(const Line& line) const
					{
						return (line.begin.x + line.end.x) + (line.begin.y + line.end.y) * 100000;
					}
				};
			};

			std::unordered_map<Line, int, Line::HashFunction, Line::Equality> lines;

			// CCW.
			static std::vector<shared::Cell> ccw_dir;
			if (ccw_dir.empty())
			{
				ccw_dir.push_back(shared::directions[shared::Directions::DownIdx]);
				ccw_dir.push_back(shared::directions[shared::Directions::RightIdx]);
				ccw_dir.push_back(shared::directions[shared::Directions::UpIdx]);
				ccw_dir.push_back(shared::directions[shared::Directions::LeftIdx]);
			}

			// Collect all lines.
			for (auto& cell : m_visited)
			{
				shared::Cell begin{ cell };

				for (auto& dir : ccw_dir)
				{
					shared::Cell end{ begin + dir };

					Line line{ begin, end };
					
					++lines[line];

					begin = end;
				}
			}

			// Remove any lines that appear more than once.
			for (auto it{ lines.begin() }; it != lines.end();)
			{
				if (it->second > 1)
					it = lines.erase(it);
				else
					++it;
			}


			struct LongLine
			{
				Line line;
				shared::Cell direction;
			};

			
			// Connect all lines that have matching begin/end and are in same direction.
			// Remove as found. Once no matches remain, that is a single side found.
			// Repeat till all lines are gone.
			int perimeter{ 0 };
			
			LongLine head;
			LongLine tail;			
			
			while (!lines.empty())
			{
				head.line = lines.begin()->first;
				head.direction = head.line.end - head.line.begin;
				tail = head;
				lines.erase(lines.begin());
				
				// Match end of tail to begin of next line.
				for (auto it{ lines.begin() }; it != lines.end();)
				{
					auto& line{ it->first };

					if (tail.line.end == line.begin && line.end - line.begin == tail.direction)
					{
						tail.line = line;
						lines.erase(it);
						it = lines.begin();
					}
					else
					{
						++it;
					}
				}				

				// Match begin of head to end of next line.
				for (auto it{ lines.begin() }; it != lines.end();)
				{
					auto& line{ it->first };

					if (head.line.begin == line.end && line.end - line.begin == head.direction)
					{
						head.line = line;
						lines.erase(it);
						it = lines.begin();
					}
					else
					{
						++it;
					}
				}

				++perimeter;
			}

			return perimeter;
		}


		size_t FindPlot(size_t x, size_t y, char crop, bool replace)
		{
			int perimeter{ 0 };
			shared::Cell cell{ static_cast<int>(x), static_cast<int>(y) };

			m_visited.clear();
			m_visited.insert(cell);

			std::queue<shared::Cell> plots;
			plots.push(cell);

			if (replace)
				m_list[y][x] = '.';

			while (!plots.empty())
			{
				auto plot{ plots.front() };
				plots.pop();

				if (m_visited.find(plot) == m_visited.end()) continue;

				for (auto& dir : shared::directions)
				{
					// Check neighbor.
					auto neighbor{ plot + dir };

					// ... out of bounds? Add perimeter.
					if (neighbor.x < 0 || neighbor.x >= m_width || neighbor.y < 0 || neighbor.y >= m_height)
					{
						++perimeter;
						continue;
					}

					auto& neighbor_crop{ m_list.at(neighbor.y).at(neighbor.x) };;

					// Same? Add to list.
					if (neighbor_crop == crop)
					{
						if (m_visited.find(neighbor) == m_visited.end())
						{
							plots.push(neighbor);
							m_visited.insert(neighbor);

							if (replace)
								neighbor_crop = '.';
						}
					}

					// Different? Add perimeter.
					else
					{
						++perimeter;
					}
				}
			}


			// Create polygon
			if (!replace)
			{
				perimeter = getSides();
			}


			/*
			if (!replace)
			{
				std::string crop_str{ crop };
				printf("Crop %s has area %d and perimeter %d = %d\n", crop_str.c_str(), area, perimeter, area * perimeter);
			}
			*/

			int area{ static_cast<int>(m_visited.size()) };
			return area * perimeter;
		}

		bool AdvanceXY()
		{
			if (++m_x >= m_width)
			{
				m_x = 0;

				if (++m_y >= m_height)
				{
					return true;
				}
			}

			return false;
		}

		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt, bool can_advance, bool go_fast) override
		{
			static bool has_found_plot{ false };

			static float elapsed_time{ 0.0f };
			static float timestep{ 0.01f };

			elapsed_time += dt;

			if (can_advance && (elapsed_time >= timestep || go_fast))
			{
				elapsed_time = 0.0f;

				while (true)
				{
					// Find first crop in line.
					if (!has_found_plot)
					{
						auto& line{ m_list[m_y] };
						if (auto t_x{ line.find_first_not_of('.', m_x) }; t_x != std::string::npos)
						{
							// Found plot.
							has_found_plot = true;

							// Set current x to found crop x.
							m_x = t_x;

							// Mark plot and tally value.
							m_total += FindPlot(m_x, m_y, m_list[m_y][m_x], false);

							Render();
							return false;
						}

						// No plots on line. Advance to next line.
						if (AdvanceXY()) return true;
					}

					// Remove found plot.
					else
					{
						// Erase plot.
						FindPlot(m_x, m_y, m_list[m_y][m_x], true);

						// Need to find next plot.
						has_found_plot = false;

						Render();
						return AdvanceXY();
					}
				}
			}

			Render();
			return false;
		}

		size_t m_x{ 0 };
		size_t m_y{ 0 };
		int m_state{ 0 };
		std::unordered_set<shared::Cell, shared::Cell::HashFunction> m_visited;
	};

	Puzzle puzzle{};

	return vizit::run(puzzle, true);
	//return vizit::run(puzzle, false); // Uses config::screen_width/height

#endif
	shared::timer(true);
}