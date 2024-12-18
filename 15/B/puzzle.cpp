#include "puzzle.h"


void Puzzle::PushSideVertical(shared::Cell<int> side, shared::Cell<int>& direction)
{
	auto destination{ side + direction };
	auto dest_letter{ warehouse[destination.x + destination.y * width] };

	// Move other boxes first.
	switch (dest_letter)
	{
		case '[':
			PushVertical(destination, destination + shared::directions<int>[shared::Directions::RightIdx], direction);
			break;

		case ']':
			PushVertical(destination + shared::directions<int>[shared::Directions::LeftIdx], destination, direction);
			break;

		default:
			break;
	}

	// Move this box.
	warehouse[destination.x + destination.y * width] = warehouse[side.x + side.y * width];
	warehouse[side.x + side.y * width] = '.';
}


void Puzzle::PushVertical(shared::Cell<int> left, shared::Cell<int> right, shared::Cell<int>& direction)
{
	PushSideVertical(left, direction);
	PushSideVertical(right, direction);
}


bool Puzzle::CanPushSideVertical(shared::Cell<int> side, shared::Cell<int>& direction)
{
	auto destination{ side + direction };
	auto dest_letter{ warehouse[destination.x + destination.y * width] };

	switch (dest_letter)
	{
		// Maybe move box
		case '.':
			return true;

			// Check other box.
		case '[':
			return CanPushVertical(destination, destination + shared::directions<int>[shared::Directions::RightIdx], direction);

			// Check other box.
		case ']':
			return CanPushVertical(destination + shared::directions<int>[shared::Directions::LeftIdx], destination, direction);

			// Box stuck - '#'
		default:
			return false;
	}
}


void Puzzle::PushHorizontal(shared::Cell<int> side, shared::Cell<int>& direction)
{
	auto destination{ side + direction * 2 };
	auto dest_letter{ warehouse[destination.x + destination.y * width] };

	// Move other boxes first.
	switch (dest_letter)
	{
		case '[':
		case ']':
			PushHorizontal(destination, direction);
			break;

		default:
			break;
	}

	// Move box.

	// ... Far side.
	auto far_side{ side + direction };
	auto far_dest{ far_side + direction };
	warehouse[far_dest.x + far_dest.y * width] = warehouse[far_side.x + far_side.y * width];
	warehouse[far_side.x + far_side.y * width] = '.';

	// ... Leading side.
	auto side_dest{ side + direction };
	warehouse[side_dest.x + side_dest.y * width] = warehouse[side.x + side.y * width];
	warehouse[side.x + side.y * width] = '.';
}


bool Puzzle::CanPushVertical(shared::Cell<int> left, shared::Cell<int> right, shared::Cell<int>& direction)
{
	return (CanPushSideVertical(left, direction) && CanPushSideVertical(right, direction));
}


bool Puzzle::CanPushHorizontal(shared::Cell<int> side, shared::Cell<int>& direction)
{
	auto destination{ side + direction * 2 };
	auto dest_letter{ warehouse[destination.x + destination.y * width] };

	switch (dest_letter)
	{
		// Maybe move box
		case '.':
			return true;

			// Check other box.
		case '[':
		case ']':
			return CanPushHorizontal(destination, direction);

			// Box stuck - '#'
		default:
			return false;
	}
}