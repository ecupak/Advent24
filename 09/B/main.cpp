// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 0

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

#include <unordered_map>
#include <limits>


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

	// block id, free spaces in block.
	std::unordered_map<size_t, int> free_space;

	// File ids in each block.
	// block[0] gives first block.
	// block[0].size() gives how many files are already in block.
	// compare to space[0] to see how many files can be in block (only useful for empty blocks).
	std::vector<std::vector<size_t>> blocks;

	constexpr size_t free_space_marker{ std::numeric_limits<size_t>::max() };

	for (auto& line : list)
	{
		size_t block_id{ 0 };
		size_t file_id{ 0 };
		bool empty_block{ false };

		for (auto& letter : line)
		{
			int space{ letter - 48 };

			// Add new block.
			if (empty_block)
			{
				blocks.emplace_back(space, free_space_marker); // Empty block.
				free_space[block_id] = space;
			}
			else
			{
				blocks.emplace_back(space, file_id++); // Block of size space with value of file_id.
				free_space[block_id] = 0;
			}

			// Next loop.
			++block_id;
			empty_block = !empty_block;
		}
	}

	// Sort it.
	size_t forward_i{ 0 };
	size_t reverse_i{ blocks.size() - 1 };

	size_t f_pos{ 0 };
	size_t r_pos{ 0 };

	while (reverse_i != 0)
	{
		// If checked all empty spaces.
		if (forward_i >= reverse_i)
		{
			forward_i = 0;
			--reverse_i;
			continue;
		}

		// Fill blocks that have space.
		if (free_space[forward_i] == 0)
		{
			++forward_i;
			continue;
		}

		// Extract from blocks that have data (no free space and not empty).
		if (free_space[reverse_i] > 0 || blocks[reverse_i].empty())
		{
			--reverse_i;
			continue;
		}

		// Get empty forward block and fill with files from end block.
		auto& r_block{ blocks[reverse_i] };

		// Can r_block fit into f_block?
		if (r_block.size() <= free_space[forward_i])
		{
			auto& f_block{ blocks[forward_i] };
			size_t f_pos{ f_block.size() - free_space[forward_i] };

			for (size_t r_pos{ 0 }; r_pos < r_block.size(); ++r_pos, ++f_pos)
			{
				std::swap(f_block[f_pos], r_block[r_pos]);
				--free_space[forward_i];
				++free_space[reverse_i];
			}
		
			forward_i = 0;
		}
		else
		{
			++forward_i;
		}
	}

	// Count results.
	size_t position{ 0 };
	for (auto& block : blocks)
	{
		for (auto value : block)
		{
			if (value != free_space_marker)
				total += value * position;

			++position;
		}

		if (block.empty()) ++position;
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
		// Can override m_width and m_height here before window is created.
		void Init() override
		{
			// Width maximum is 200			
			unsigned int width{ 0 };
			unsigned int height{ 0 };

			// Sets SFML font / text data.
			m_font_size = 8;
			Challenge::Init();

			std::vector<size_t> px_spaces;

			for (auto& line : m_list)
			{
				size_t block_id{ 0 };
				size_t file_id{ 0 };
				bool empty_block{ false };

				for (auto& letter : line)
				{
					int space{ letter - 48 };
					width += space;
					points += space;
					px_spaces.push_back(space);
					if (width > max_width)
					{
						width = space;
						++height;
					}

					// Add new block.
					if (empty_block)
					{
						blocks.emplace_back(space, free_space_marker); // Empty block.
						free_space[block_id] = space;
					}
					else
					{
						blocks.emplace_back(space, file_id++); // Block of size space with value of file_id.
						free_space[block_id] = 0;
					}

					// Next loop.
					++block_id;
					empty_block = !empty_block;
				}
			}

			m_width = width;
			m_height = height + 2;

			// Set starting reverse iter.
			reverse_i = blocks.size() - 1;

			// Build point array.
			unsigned int x{ 0 };
			unsigned int y{ 0 };
			px_points = sf::VertexArray(sf::Points, points);		
			for (size_t i{ 0 }; i < px_spaces.size(); ++i)
			{
				x = 0;

				for (size_t k{ 0 }; k < px_spaces[i]; ++k)
				{
					px_points[i].position = sf::Vector2f(x, y);
					px_points[i].color = free_space[i] != 0 ? sf::Color::White : sf::Color::Black;
					++x;
				}
				++y;
			}
		}

		// Drawing to the window.
		void Render()
		{			

			size_t row{ 0 };			
			size_t length{ 0 };
			size_t cursor{ 0 };
			for (int i{ 0 }; i < blocks.size(); ++i)
			{
				length += blocks[i].size();
				if (length > max_width)
				{
					length = blocks[i].size();
					cursor = 0;
					++row;
				}

				if (match_found && (i == forward_i || i == reverse_i))
					m_text.setFillColor(sf::Color::Green);
				else if (i == forward_i)
					m_text.setFillColor(sf::Color::Cyan);
				else if (i == reverse_i)
					m_text.setFillColor(sf::Color::Yellow);
				else
					m_text.setFillColor(sf::Color(150, 150, 150, 255));

				auto filled_spaces{ blocks[i].size() - free_space[i] };

				for (int f{ 0 }; f < blocks[i].size(); ++f)
				{
					//m_text.setString(f < filled_spaces ? std::to_string(blocks[i][0]) : ".");
					m_text.setString(f < filled_spaces ? 'F' : '.');
					m_text.setPosition({ static_cast<float>(cursor * m_font_size), static_cast<float>(row * (m_font_size + 4)) });
					m_window->draw(m_text);
					++cursor;
				}

				if (blocks[i].empty())
				{
					m_text.setString(',');
					m_text.setPosition({ static_cast<float>(cursor * m_font_size - 8), static_cast<float>(row * m_font_size + 2) });
					m_window->draw(m_text);
				}
			}
		}

		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt, bool can_advance) override
		{
			static float elapsed_time{ 0.0f };
			static float timestep{ 0.2f };

			elapsed_time += dt;
			
			if (can_advance && elapsed_time >= timestep)
			{
				elapsed_time = 0;

				static size_t f_pos{ 0 };
				static size_t r_pos{ 0 };

				static bool advance_fi{ false };
				static bool reset_fi{ false };

				while (reverse_i != 0)
				{
					if (advance_fi)
					{
						++forward_i;
						advance_fi = false;
					}

					if (reset_fi)
					{
						forward_i = 1;
						reset_fi = false;
						//printf("Reset fwd i\n");
						return false;
					}

					if (!match_found)
					{
						// If checked all empty spaces.
						if (forward_i >= reverse_i)
						{
							forward_i = 1;
							--reverse_i;
							//printf("Fwd i >= Rev i; reset fwd, --rev\n");
							while (free_space[reverse_i] > 0 || blocks[reverse_i].empty())
							{
								--reverse_i;
								//printf("wl: --rev i\n");
							}
							return false;
						}

						// Fill blocks that have space.
						if (free_space[forward_i] == 0)
						{
							++forward_i;
							//printf("++fwd i\n");
							continue;
						}

						// Extract from blocks that have data (no free space and not empty).
						if (free_space[reverse_i] > 0 || blocks[reverse_i].empty())
						{
							--reverse_i;
							//printf("--rev i\n");
							return false;
						}
					}

					// Get empty forward block and fill with files from end block.
					auto& r_block{ blocks[reverse_i] };

					// Can r_block fit into f_block?
					if (r_block.size() <= free_space[forward_i])
					{
						if (!match_found)
						{
							match_found = true;
							Render();
							//printf("match found\n");
							return false;
						}

						auto& f_block{ blocks[forward_i] };
						size_t f_pos{ f_block.size() - free_space[forward_i] };

						for (size_t r_pos{ 0 }; r_pos < r_block.size(); ++r_pos, ++f_pos)
						{
							std::swap(f_block[f_pos], r_block[r_pos]);
							--free_space[forward_i];
							++free_space[reverse_i];
						}

						//forward_i = 0;
						reset_fi = true;

						match_found = false;
						Render();
						//printf("match swapped\n");
						return false;
					}
					else
					{
						//++forward_i;
						advance_fi = true;
						//printf("no match; ++fwd i\n");
						return false;
					}
				}
			}
			else
			{
				Render();
				return false;
			}

			Render();
			return true;
		}

		size_t forward_i{ 0 };
		size_t reverse_i{ 0 };
		bool match_found{ false };
		size_t points{ 0 };
		sf::VertexArray px_points;

		unsigned int max_width{ 400 };

		std::unordered_map<size_t, int> free_space;
		std::vector<std::vector<size_t>> blocks;
		const size_t free_space_marker{ std::numeric_limits<size_t>::max() };
	};

	Puzzle puzzle{};

	// run(puzzle, true) => uses puzzle's calculated width/height.
	// run(puzzle, false) => uses config setting width/height.	
	return vizit::run(puzzle, true);

#endif
	shared::timer(true);
}