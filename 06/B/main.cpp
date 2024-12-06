#define SKIP_VISUALIZE true

#include "shared.h"

#include <iostream>
#include <vector>

#include <unordered_set>


unsigned int makeHash(int x, int y)
{
	return x + y * 10000;
}


int main()
{
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
	shared::Cell start_cell{ };

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
					spaces.insert(makeHash(x, y));
					break;
				case '^':
					spaces.insert(makeHash(x, y));
					start_cell = shared::Cell{ x, y };
					break;
			}
			++x;
		}
		++y;
	}

	int width{ x };
	int height{ y };

	// Explore map.
	{
		shared::Cell cell{ start_cell };
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
	}


	// Now place an obstacle on the visited path and count how many loops are created.
	{
		std::unordered_set<unsigned int> loops;
		auto iter2{ visited.begin() };
		++iter2; // Skip starting location.

		for (; iter2 != visited.end(); ++iter2)
		{
			if (*iter2 == makeHash(start_cell.x, start_cell.y)) 
			{
				continue;
			}

			unsigned int hash_value{ *iter2 };
			bool loop_found{ false };

			// Reset values.
			loops.clear();
			shared::Cell cell{ start_cell };
			unsigned int next_cell_hash{ 0 };
			int dir{ shared::Directions::UpIdx };
			shared::Cell direction{ shared::directions[dir] };

			// Modify lists with temp new obstacle.
			obstacles.insert(hash_value);
			spaces.erase(hash_value);

			// Normal check as before with loop detection.
			while (true)
			{
				shared::Cell next_cell{ cell + direction };
				next_cell_hash = makeHash(next_cell.x, next_cell.y);

				// See if empty cell.
				if (auto iter{ spaces.find(next_cell_hash) }; iter != spaces.end())
				{
					// Move to next cell.
					cell = next_cell;
				}

				// See if obstacle.
				else if (auto iter{ obstacles.find(next_cell_hash) }; iter != obstacles.end())
				{
					// Rotate until no obstacle.
					shared::Cell maybe_next_cell{ cell + direction };
					unsigned int maybe_next_cell_hash{ makeHash(maybe_next_cell.x, maybe_next_cell.y) };

					while (obstacles.find(maybe_next_cell_hash) != obstacles.end())
					{
						// Add to set using direction as hash element.
						// If set already has it, this is a loop. 
						// That is, if you hit the same obstacle twice from the same direction, you are in a loop.
						if (!loops.insert(makeHash(dir, *iter)).second)
						{
							loop_found = true;
							printf("%d with %d\n", *iter, hash_value);
							break;
						}

						// Rotate.
						switch (dir)
						{
							case 0: dir = 1; break;
							case 1: dir = 2; break;
							case 2: dir = 3; break;
							case 3: dir = 0; break;
						}

						direction = shared::directions[dir];

						// Test new direction.
						maybe_next_cell = cell + direction;
						maybe_next_cell_hash = makeHash(maybe_next_cell.x, maybe_next_cell.y);
					}

					if (loop_found)
					{
						total += 1;
						break;
					}
				}

				// Neither - off map!
				else
				{
					break;
				}
			};

			// Restore lists.
			obstacles.erase(hash_value);
			spaces.insert(hash_value);
		}
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);
}