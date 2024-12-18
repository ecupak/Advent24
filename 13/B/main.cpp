// 0 is off, 1 is on, 2 is on but don't render (use in release when you just want speed).
#define VISUAL_MODE 0 

// False will use the test file "input - Copy.txt"
#define USE_INPUT_FILE false


#include "shared.h"

#include <iostream>
#include <vector>

// Regex.
#include <iostream>
#include <iterator>
#include <regex>

int main()
{
	shared::timer();

#if VISUAL_MODE == 0
	// PARSE INPUT FILE //

	std::vector<std::string> list;
	shared::InputResult result{ shared::parseInputFile(list, true) };
	if (result == shared::BAD) return 2;

	// SOLVE PUZZLE //

	long long total{ 0 };

	// std::vector<int> values;

	long long cost_a{ 3 };
	long long cost_b{ 1 };

	shared::Cell<long long> btn_a;
	shared::Cell<long long> btn_b;
	shared::Cell<long long> prize;
	int check_state{ 0 };

	for (auto& line : list)
	{
		if (line.empty()) continue;

		std::string regex_str{ "" };

		switch (check_state)
		{
			case 0:
				regex_str = "(X\\+\\d+|Y\\+\\d+)";
				break;
			case 1:
				regex_str = "(X\\+\\d+|Y\\+\\d+)";
				break;
			case 2:
				regex_str = "(X\\=\\d+|Y\\=\\d+)";
				break;
		}

		std::regex instruction_regex{ regex_str };
		auto instruction_begin{ std::sregex_iterator(line.begin(), line.end(), instruction_regex) };
		auto instruction_end{ std::sregex_iterator() };

		std::vector<long long> nums;

		for (std::sregex_iterator i{ instruction_begin }; i != instruction_end; ++i)
		{
			auto str{ (*i).str() };

			str = str.substr(2);

			nums.push_back(std::stoll(str));
		}

		switch (check_state)
		{
			case 0:
				btn_a.x = nums[0];
				btn_a.y = nums[1];
				break;
			case 1:
				btn_b.x = nums[0];
				btn_b.y = nums[1];
				break;
			case 2:
				prize.x = nums[0] + 10000000000000;
				prize.y = nums[1] + 10000000000000;
				break;
		}

		// Repeat until all info is gathered.
		if (++check_state < 3) continue;

		// Linear algebra to the rescue.
		// NEVERMIND THIS DID NOT WORK.
		{
			/*
			float slope_a{ 1.0f * btn_a.y / 1.0f * btn_a.x };
			float slope_b{ 1.0f * btn_b.y / 1.0f * btn_b.x };

			float x_a = ((-1.0f * prize.x * slope_b) + 1.0f * prize.y) / (slope_a - slope_b);
			float x_b = ((-1.0f * prize.x * slope_a) + 1.0f * prize.y) / (slope_b - slope_a);

			long long la_btn_a_count{ static_cast<long long>(x_a / btn_a.x) };
			long long la_btn_b_count{ static_cast<long long>(x_b / btn_b.x) };

			// Confirm truncated values reach prize and that we didn't go over 100 presses.
			bool reaches_prize{ btn_a.x * btn_count_a + btn_b.x * btn_count_b == prize.x && btn_a.y * btn_count_a + btn_b.y * btn_count_b == prize.y };
			bool under_max_100{ btn_count_a <= 100 && btn_count_b <= 100 };

			if (reaches_prize && under_max_100)
			{
				//total += cost_a * btn_count_a + cost_b * btn_count_b;
			}
			*/
		}

		
		// Using Cramer's Rule to solve 2 systems with 2 unknowns.
		// https://www.reddit.com/r/adventofcode/comments/1hd7irq/2024_day_13_an_explanation_of_the_mathematics/
		
		// btn_A.x * btn_A_count + btn_B.x * btn_B_count = prize.x
		// btn_A.y * btn_A_count + btn_B.y * btn_B_count = prize.y

		// Setup.
		/*
			| btn_A.x	btn_B.x |
			|					| = Determinant (D)
			| btn_A.y	btn_B.y	|


			| x |				| prize.x |
			|   | = variables	|		  | = constants
			| y |				| prize.y |


			| prize.x	btn_B.x |
			|					| = Determinant_X (Dx)
			| prize.y	btn_B.y	|


			| btn_A.x	prize.x |
			|					| = Determinant_Y (Dy)
			| btn_A.y	prize.y	|


			x = Dx / D
			y = Dy / D
		*/

		// Solving a square matrix.
		/*
			| A B |
			| C	D | = A*D - B*C
		*/

		long long det  { btn_a.x * btn_b.y - btn_b.x * btn_a.y };
		long long det_x{ prize.x * btn_b.y - btn_b.x * prize.y };
		long long det_y{ btn_a.x * prize.y - prize.x * btn_a.y };

		long long btn_a_count{ det_x / det };
		long long btn_b_count{ det_y / det };

		long long p_x{ btn_a_count * btn_a.x + btn_b_count * btn_b.x };
		long long p_y{ btn_a_count * btn_a.y + btn_b_count * btn_b.y };

		// Make sure solution reaches prize (may need a fractional answer).		
		if (p_x == prize.x && p_y == prize.y)
		{
			total += static_cast<long long>(btn_a_count * 3ll + btn_b_count);
		}

		check_state = 0;
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
		// LoadFile() called before Init().
		void Init() override
		{
			// Sets SFML font / text data.
			m_font_size = 10;
			Challenge::Init();

			// Set how fast the animation should play.
			m_timestep = 0.01f;

			// Build initial data structure from input.
			for (auto& line : m_list)
			{
				// shared::getSpaceDelimitedValuesFromLine<int>(line, values);
			}
		}


		// Return true when puzzle is solved. Will pause process.
		bool Tick(float dt) override
		{
			return true;
		}


		// Drawing to the window. Called after Tick().
		void Render()
		{
			m_text.setString("Exaple text.");
			m_text.setFillColor(sf::Color(150, 150, 150, 255));
			m_text.setFillColor(sf::Color::Green);

			m_text.setPosition({ static_cast<float>(x * m_font_size), static_cast<float>(y * m_font_size) });
			m_window->draw(m_text);
		}


		// std::vector<int> values;		
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