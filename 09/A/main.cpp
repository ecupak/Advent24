// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 0 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE true


#include "shared.h"

#include <iostream>
#include <vector>

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

	// Chunk id (not file id), block size.
	std::vector<bool> empty_tracker;
	
	// File ids in each block.
	// block[0] gives first block.
	// block[0].size() gives how many files are already in block.
	// compare to space[0] to see how many files can be in block (only useful for empty blocks).
	std::vector<std::vector<unsigned int>> blocks;

	for (auto& line : list)
	{
		unsigned int file_id{ 0 };
		bool empty_block{ false };

		for (auto& letter : line)
		{
			// Add new block.
			if (empty_block)
				blocks.emplace_back(letter - 48, 0); // Empty block.
			else
				blocks.emplace_back(letter - 48, file_id++); // Block of size space[block_id] with value of file_id.

			// Track if block is empty.
			empty_tracker.push_back(empty_block);

			// Next loop.
			empty_block = !empty_block;
		}
	}

	// Sort it.
	size_t forward_i{ 0 };
	size_t reverse_i{ blocks.size() - 1 };
	bool empty_block{ false };

	size_t f_pos{ 0 };
	size_t r_pos{ 0 };

	while (forward_i < reverse_i)
	{
		// Can't fill in blocks that are full.
		if (!empty_tracker[forward_i])
		{
			++forward_i;
			continue;
		}

		// Can't extract from blocks that are empty.
		if (empty_tracker[reverse_i])
		{
			--reverse_i;
			continue;
		}

		// Get empty forward block and fill with files from end block.
		auto& f_block{ blocks[forward_i] };
		auto& r_block{ blocks[reverse_i] };

		for (; f_pos < f_block.size() && r_pos < r_block.size(); ++f_pos, ++r_pos)
		{
			std::swap(f_block[f_pos], r_block[r_pos]);
		}

		// Get next end block and reset pos.
		if (r_pos == r_block.size())
		{
			r_pos = 0;
			--reverse_i;			
		}
		
		// Get next forward block and reset pos.
		if (f_pos == f_block.size())
		{			
			f_pos = 0;
			++forward_i;
		}
	}

	// Reverse last end block used (turns ..6 into 6..).
	auto& r_block{ blocks[reverse_i] };
	std::reverse(r_block.begin(), r_block.end());

	// Count results.
	size_t position{ 0 };
	for (auto& block : blocks)
	{
		for (auto value : block)
		{
			total += value * position++;
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