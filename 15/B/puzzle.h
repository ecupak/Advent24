#pragma once

#include "shared.h"


class Puzzle : public vizit::Challenge
{
public:
	bool CanPushHorizontal(shared::Cell<int> side, shared::Cell<int>& direction);
	void PushHorizontal(shared::Cell<int> side, shared::Cell<int>& direction);
	bool CanPushVertical(shared::Cell<int> left, shared::Cell<int> right, shared::Cell<int>& direction);
	bool CanPushSideVertical(shared::Cell<int> side, shared::Cell<int>& direction);
	void PushVertical(shared::Cell<int> left, shared::Cell<int> right, shared::Cell<int>& direction);
	void PushSideVertical(shared::Cell<int> side, shared::Cell<int>& direction);

	// LoadFile() called before Init().
	void Init() override
	{
		// Sets SFML font / text data.
		m_font_size = 24;
		Challenge::Init();

		// Set how fast the animation should play.
		m_anim_delay = 0.00f;

		// Build initial data structure from input.
		int y{ 0 };
		int x{ 0 };
		m_height = 0;

		for (auto& line : m_list)
		{
			if (line[0] == '#')
			{
				int pos{ 0 };

				for (auto letter : line)
				{
					switch (letter)
					{
						case '.':
							warehouse.append("..");
							break;
						case '#':
							warehouse.append("##");
							break;
						case 'O':
							warehouse.append("[]");
							break;
						case '@':
							robot.x = pos;
							robot.y = y;
							warehouse.append("..");
							break;
						default:
							break;
					}

					++pos;
					++pos;
				}

				x = static_cast<int>(line.size() * 2);

				++m_height;
			}
			else
			{
				inputs += line;
			}

			++y;
		}

		m_width = x;
		++m_height;

		width = m_width;
		height = m_height - 1;
	}


	bool TryPush(shared::Cell<int> left, shared::Cell<int> right, shared::Cell<int>& direction)
	{
		// Move horizontally.
		if (direction.x != 0)
		{
			// Push right (use left side).
			if (direction.x == 1)
			{
				if (CanPushHorizontal(left, direction))
				{
					PushHorizontal(left, direction);
					return true;
				}
			}
			else
			{
				if (CanPushHorizontal(right, direction))
				{
					PushHorizontal(right, direction);
					return true;
				}
			}
		}
		else
		{
			if (CanPushVertical(left, right, direction))
			{
				PushVertical(left, right, direction);
				return true;
			}
		}

		return false;
	}


	bool MoveRobot(int dir)
	{
#if VISUAL_MODE == 1
		shadows.write(robot);
#endif
		auto& direction{ shared::directions<int>[dir] };

		auto destination{ robot + direction };

		auto letter{ warehouse[destination.x + destination.y * width] };

		switch (letter)
		{
			// Move
			case '.':
				robot = destination;
				return true;

				// Push
			case '[':
				if (TryPush(destination, destination + shared::directions<int>[shared::Directions::RightIdx], direction))
				{
					robot = destination;
					return true;
				}
				else
				{
					return false;
				}

			case ']':
				if (TryPush(destination + shared::directions<int>[shared::Directions::LeftIdx], destination, direction))
				{
					robot = destination;
					return true;
				}
				else
				{
					return false;
				}
				// Stay
			default: // #
				return false;
		}
	}


	// Return true when puzzle is solved. Will pause process.
	bool Tick(float dt) override
	{
		auto input{ inputs[++input_idx] };

		switch (input)
		{
			case '<':
				input_outcome = MoveRobot(shared::Directions::LeftIdx);
				break;
			case '>':
				input_outcome = MoveRobot(shared::Directions::RightIdx);
				break;
			case '^':
				input_outcome = MoveRobot(shared::Directions::UpIdx);
				break;
			case 'v':
				input_outcome = MoveRobot(shared::Directions::DownIdx);
				break;
			default:
				input_outcome = false;
				break;
		}

		if (input_idx + 1 >= inputs.size())
		{
			// Calculate score.
			for (unsigned int y{ 0 }; y < height; ++y)
			{
				auto y_offset{ y * width };

				for (unsigned int x{ 0 }; x < width; ++x)
				{
					if (warehouse[x + y_offset] == '[')
					{
						m_total += x + y * 100;
					}
				}
			}

			return true;
		}
		else
		{
			return false;
		}
	}


	// Drawing to the window. Called after Tick().
	void Render()
	{
		// Past positions.
		m_text.setString('@');

		unsigned int alpha{ 160u - (10 * static_cast<unsigned int>(shadows.size())) };
		for (const auto& shadow : shadows)
		{
			m_text.setFillColor(sf::Color(0, 150, 150, alpha));
			m_text.setPosition({ static_cast<float>(shadow.x * m_font_size), static_cast<float>(shadow.y * m_font_size) });
			m_window->draw(m_text);

			alpha += 10;
		}

		// Map
		for (size_t y{ 0 }; y < height; ++y)
		{
			auto y_offset{ y * width };

			for (size_t x{ 0 }; x < width; ++x)
			{
				auto letter{ warehouse[x + y_offset] };
				m_text.setString(letter);

				switch (letter)
				{
					case '#':
						m_text.setFillColor(sf::Color::White);
						break;
					case 'O':
					case '[':
					case ']':
						m_text.setFillColor(sf::Color::Yellow);
						break;
					default:
						m_text.setFillColor(sf::Color(150, 150, 150, 255));
						break;
				}

				m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
				m_window->draw(m_text);
			}
		}

		// Robot
		m_text.setString('@');
		m_text.setFillColor(sf::Color::Cyan);
		m_text.setPosition({ static_cast<float>(robot.x * m_font_size), static_cast<float>(robot.y * m_font_size) });
		m_window->draw(m_text);

		// Input line
		static size_t mid{ (width / 2) - 1 };
		static size_t rem{ width - 1 - mid };

		// .. Show consumed inputs.
		m_text.setFillColor(sf::Color(150, 150, 150, 255));
		for (size_t x{ 0 }; x < mid; ++x)
		{
			int old_idx{ static_cast<int>(input_idx - mid + x) };
			if (old_idx >= 0)
			{
				m_text.setString(inputs[old_idx]);
				m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(height * m_font_size) });
				m_window->draw(m_text);
			}
		}

		// .. Show current input.
		if (input_idx >= 0)
		{
			m_text.setString(inputs[input_idx]);
			m_text.setFillColor(input_outcome ? sf::Color::Green : sf::Color::Red);
			m_text.setPosition({ static_cast<float>(mid * m_font_size), static_cast<float>(height * m_font_size) });
			m_window->draw(m_text);
		}

		// .. Show upcoming inputs.
		// Immediate is yellow.
		m_text.setFillColor(sf::Color::Yellow);
		for (size_t x{ 0 }; x < rem; ++x)
		{
			int next_idx{ static_cast<int>(input_idx + x + 1) };
			if (next_idx < inputs.size())
			{
				m_text.setString(inputs[next_idx]);
				m_text.setPosition({ static_cast<float>((mid + x + 1) * m_font_size), static_cast<float>(height * m_font_size) });
				m_window->draw(m_text);
			}
			m_text.setFillColor(sf::Color(180, 180, 180, 255));
		}
	}

	int input_idx{ -1 };
	bool input_outcome{ false };

	std::string warehouse;
	std::string inputs;
	shared::Cell<int> robot;
	container::RingBuffer<shared::Cell<int>> shadows{ 10 };

	size_t width{ 0 };
	size_t height{ 0 };
};