#pragma once

// General file reading.
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// Parsing based on delimiter.
#include <sstream>
#include <algorithm>

// Time checking.
#include <chrono>

// For SFML when using rendering loop.
#include "SFML/Graphics.hpp"
#include <SFML/System/Clock.hpp>


namespace shared
{
	std::string path{ "../../shared/" };

	enum InputResult
	{
		GOOD = 0,
		WARNING = 1,
		BAD = 2,
	};

	/*
		DECLARATIONS
	*/
	InputResult parseInputFile(std::vector<std::string>& list, bool as_lines);
	bool parseInputFromFile(const char* filename, std::vector<std::string>& list, bool as_lines);
	void fillListWithInput(std::ifstream& input, std::vector<std::string>& list);
	void fillListWithLines(std::ifstream& input, std::vector<std::string>& list);
	void timer(bool stop = false);

	template <typename T>
	void getSpaceDelimitedValuesFromLine(const std::string& line, std::vector<T>& values);

	/*
		DEFINITIONS
	*/

	// 'as_lines' true will copy every input line as-is. If false, will copy individual values separated by space/linebreak.
	InputResult parseInputFile(std::vector<std::string>& list, bool as_lines)
	{
#ifndef USE_INPUT_FILE
#define USE_INPUT_FILE true
#endif

#if USE_INPUT_FILE
		if (parseInputFromFile("../input.txt", list, as_lines))
		{
			return GOOD;
		}
#else
		if (parseInputFromFile("../input - Copy.txt", list, as_lines))
		{
			return WARNING;
		}
#endif
		std::cerr << "Couldn't find input file.\n";
		return BAD;
	}


	bool parseInputFromFile(const char* filename, std::vector<std::string>& list, bool as_lines)
	{
		if (std::ifstream input{ filename }; input)
		{
			if (as_lines)
				fillListWithLines(input, list);
			else
				fillListWithInput(input, list);
			return true;
		}

		return false;
	}


	void fillListWithInput(std::ifstream& input, std::vector<std::string>& list)
	{
		std::string strInput;

		while (input >> strInput)
		{
			list.push_back(strInput);
		}
		input.close();
	}


	void fillListWithLines(std::ifstream& input, std::vector<std::string>& list)
	{
		while (!input.eof())
		{
			std::string value;
			std::getline(input, value, '\n');

			if (value.empty()) continue;

			list.push_back(value);
		}
	}


	// [Credit] https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c/22387757#22387757 + Alex
	void timer(bool stop)
	{
		using std::chrono::high_resolution_clock;
		using std::chrono::duration_cast;
		using std::chrono::duration;
		using std::chrono::milliseconds;

		static auto t1 = high_resolution_clock::now();
		static auto t2 = t1;

		if (stop)
		{
			t2 = high_resolution_clock::now();

			// Getting number of milliseconds as a double.
			duration<double, std::milli> ms_double = t2 - t1;

			std::cout << "Completed in: " << ms_double.count() << "ms\n";
		}
		else
		{
			t1 = high_resolution_clock::now();
		}
	}


	template <typename T>
	void getSpaceDelimitedValuesFromLine(const std::string& line, std::vector<T>& values)
	{
		std::istringstream full_line{ line };

		while (!full_line.eof())
		{
			T value;

			full_line >> value;

			values.push_back(value);
		}
	}


	struct Cell
	{
		Cell() = default;

		Cell(int _x, int _y)
			: x{ _x }
			, y{ _y }
		{	}

		int x{ 0 };
		int y{ 0 };

		Cell operator+(const Cell& rhs)
		{
			return { this->x + rhs.x, this->y + rhs.y };
		}

		Cell operator-(const Cell& rhs)
		{
			return { this->x - rhs.x, this->y - rhs.y };
		}

		Cell& operator+=(const Cell& rhs)
		{
			this->x += rhs.x;
			this->y += rhs.y;

			return *this;
		}

		bool operator==(const Cell& other) const
		{
			return (x == other.x && y == other.y);
		}

		std::string toString() const 
		{
			return std::to_string(x) + ", " + std::to_string(y);
		}

		struct HashFunction
		{
			size_t operator()(const Cell& cell) const
			{
				return std::hash<std::string>()(cell.toString());
			}
		};
	};


	Cell directions[4]{
		{  0, -1 },
		{  1,  0 },
		{  0,  1 },
		{ -1,  0 },
	};


	enum Directions
	{
		UpIdx = 0,
		RightIdx,
		DownIdx,
		LeftIdx,
		Count,
	};


	unsigned int makeHash(int x, int y)
	{
		return x + (y << 16);
	}

	void unhashMe(unsigned int hash, int& x, int& y)
	{
		x = hash & 0x0000FFFF;
		y = hash >> 16;
	}


	// [Credit] https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer
	template <class T>
	int numDigits(T number)
	{
		int digits = 0;
		//if (number < 0) digits = 1; // remove this line if '-' counts as a digit
		while (number) {
			number /= 10;
			++digits;
		}
		return digits;
	}

}// namespace shared


namespace config {
	const int screen_width{ 600 };
	const int screen_height{ 480 };

	const int target_fps{ 60 };
	const float target_fps_as_delta_time{ 1.0f / target_fps };

	const int minimum_fps{ 10 };
	const float maximum_delta_time{ 1.0f / minimum_fps };
} // namespace config


namespace vizit
{

class Challenge
{
public:
	int LoadFile()
	{
		shared::InputResult result{ shared::parseInputFile(m_list, true) };

		try
		{
			m_width = static_cast<int>(m_list.at(0).size());
			m_height = static_cast<int>(m_list.size());
		}
		catch (...)
		{
			printf("Could not set width/height.");
		}

		return result;
	}

	virtual void Init() { 
		std::string font_path{ shared::path + "Bittypix/Bittypix_Monospace.otf" };
		if (!m_font.loadFromFile(font_path))
		{
			printf("Could not load font from \"%s\"", font_path.c_str());
		}
		m_text.setFont(m_font);
		m_text.setCharacterSize(m_font_size);
		m_text.setFillColor(sf::Color::White); 
	}

	virtual bool Tick(float dt, bool can_advance, bool go_fast = false) { return true; }

	// Rendering context.
	sf::RenderWindow* m_window;

	// Input parsed as a string. Final answer.
	std::vector<std::string> m_list;
	unsigned int m_total{ 0 };
	
	// # of letters per line and # of lines. Set during init, can be overwritten as needed.
	int m_width{ 0 };
	int m_height{ 0 };

	// Render strings using this.
	sf::Font m_font;
	sf::Text m_text;
	unsigned int m_font_size{ 24 };
};


int run(Challenge& challenge, bool use_puzzle_size)
{
#ifndef VISUAL_MODE
#define VISUAL_MODE 1
#endif

	// Initialize challenge.
	int result{ challenge.LoadFile() };

	if (result == shared::BAD)
	{
		return 1;
	}
	else
	{
		challenge.Init();
	}

	// Create window.
	unsigned int screen_width{ static_cast<unsigned int>(use_puzzle_size ? challenge.m_width * challenge.m_font_size : config::screen_width) };
	unsigned int screen_height{ static_cast<unsigned int>(use_puzzle_size ? challenge.m_height * challenge.m_font_size : config::screen_height) };
	sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Challenge Begin!");
	challenge.m_window = &window;

	// Loop variables.
#if VISUAL_MODE == 2
	bool skip_wait{ true };
#else
	bool skip_wait{ false };
#endif
	bool can_advance{ false };
	bool is_running{ true };
	bool is_done{ false };

	// Main loop.
	{
		sf::Clock clock;
		float delta_time{ 0.0f };

#if VISUAL_MODE == 2
		while (!challenge.Tick(0.0f, true, true));
#else
		while (window.isOpen())
		{
			// Only advance game when enough time has elapsed.
			delta_time += clock.restart().asSeconds();
						
			if (delta_time >= config::target_fps_as_delta_time)
			{
				// Close window when done.
				if (!is_running)
				{
					window.close();
					continue;
				}

				// Clear window.
				window.clear();

				// Game tick.		
				delta_time = fminf(config::maximum_delta_time, delta_time);

				if (challenge.Tick(delta_time, can_advance))
				{
					is_done = true;
					
					// Pause at end of execution.
					skip_wait = false;
				}

				// Update title with new total.
				std::string title{ "Total: " + std::to_string(challenge.m_total) };
				window.setTitle(title);

				// Reset advancement check. Auto-true when not waiting.
				can_advance = skip_wait;

				// Event handling.
				sf::Event event;
				while (window.pollEvent(event))
				{
					switch (event.type)
					{
						case sf::Event::Closed:
							window.close();
							break;

						case sf::Event::Resized:
							window.setSize(sf::Vector2u{ event.size.width, event.size.height });
							break;

						case sf::Event::MouseButtonPressed:
							can_advance = true;
							break;

						case sf::Event::KeyPressed:
							if (event.key.scancode == sf::Keyboard::Scan::Enter)
							{
								skip_wait = !skip_wait;
							}

							can_advance = true;
							break;
					}
				}

				// Once complete, wait for final input to terminate.
				is_running = !(is_done && can_advance);

				// Show the new render (swap double buffer).
				window.display();

				delta_time = 0.0f;//-= config::target_fps_as_delta_time;
			}
		}
#endif
	}

	// Answer!
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;
	std::cout << challenge.m_total << std::endl;
	if (result == shared::WARNING) std::cout << "USING TEST FILE!" << std::endl;

	shared::timer(true);

	return 0;
}

} // namespace vizit


// [Credit] rathbhupendra // https://www.geeksforgeeks.org/print-all-permutations-with-repetition-of-characters/
namespace permutation
{
	/* The main function that recursively
	prints all repeated permutations of
	the given string. It uses data[] to store all
	permutations one by one */
	// [Private method]
	void createCombinations(std::vector<std::string>& results, std::string& given_str, char* data, uint64_t last, int index)
	{
		// One by one fix all characters at the given index
		// and recursively for the subsequent indexes. 
		for (int i{ 0 }; i < given_str.size(); ++i)
		{
			// Fix the n-th character at index and if this is not the last 
			// index then recursively call for higher indexes.
			data[index] = given_str[i];

			// Recursively call for higher indexes.
			if (index == last)
				results.push_back(data);
			else
				createCombinations(results, given_str, data, last, index + 1);
		}
	}

	/* This function sorts input string,
	allocate memory for data (needed
	for allLexicographicRecur()) and
	calls allLexicographicRecur() for
	printing all permutations */
	void get(std::vector<std::string>& results, std::string& given_str, size_t length = 0)
	{
		uint64_t combo_length{ length > 0 ? length : static_cast<uint64_t>(given_str.size() - 1u) };

		// Create a temp array to build combination in. 
		char* data{ new char[combo_length + 1] };
		std::fill_n(data, combo_length, '.');
		data[combo_length] = '\0';

		// Now print all permutations 
		createCombinations(results, given_str, data, combo_length - 1, 0);

		delete[] data;
	}
}