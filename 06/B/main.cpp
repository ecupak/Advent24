#define SKIP_VISUALIZE true
#define USE_INPUT_FILE false

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

	std::vector<unsigned int> path_hashes;
	size_t steps_taken{ 0 };

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
				path_hashes.push_back(cell_hash);
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
		// All coords visited.
		unsigned int start_hash{ makeHash(start_cell.x, start_cell.y) };
		for (auto v_iter{ visited.begin() }; v_iter != visited.end(); ++v_iter)
		{
			// Don't place obstacle on start cell.
			if (*v_iter == start_hash) continue;

			// Add temp obstacle on map along path.
			bool placed_obstacle{ false };
			unsigned int temp_obstacle_hash{ *v_iter };

			if (obstacles.find(temp_obstacle_hash) == obstacles.end())
			{
				placed_obstacle = true;
				spaces.erase(temp_obstacle_hash);
				obstacles.insert(temp_obstacle_hash);
			}

			// Reset values.
			// ... Tracks if loop was found.
			bool loop_found{ false };
			std::unordered_set<unsigned int> loops;

			// ... Path position and direction.
			shared::Cell cell{ start_cell };
			unsigned int next_cell_hash{ 0 };
			int dir{ shared::Directions::UpIdx };
			shared::Cell direction{ shared::directions[dir] };

			// Normal check as before with loop detection and temp obstacle placement.
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
						if (!loops.insert(makeHash(dir, maybe_next_cell_hash)).second)
						{
							loop_found = true;
							//printf("Repeat visit at: %05d | Dir: %d | Obstacle at: %d\n", maybe_next_cell_hash, dir, temp_obstacle_hash);
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

					} // Looped direction check for obstacles.

					if (loop_found)
					{
						total += 1;
						break;
					}

				} // Obstacle set check.

				// Neither - off map!
				else
				{
					break;
				}

			} // Path while-loop

			// Restore lists if temp obstacle was placed.
			if (placed_obstacle)
			{
				spaces.insert(temp_obstacle_hash);
				obstacles.erase(temp_obstacle_hash);
			}

		} // Obstacle iteration for-loop
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);
}