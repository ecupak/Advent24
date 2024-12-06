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
	InputResult parseInputFile(std::vector<std::string>& list);
	bool parseInputFromFile(const char* filename, std::vector<std::string>& list);
	void fillListWithInput(std::ifstream& input, std::vector<std::string>& list);
	void timer(bool stop = false);

	template <typename T>
	void getSpaceDelimitedValuesFromLine(const std::string& line, std::vector<T>& values);

	/*
		DEFINITIONS
	*/
	InputResult parseInputFile(std::vector<std::string>& list)
	{
		// Check for test file first.
		if (parseInputFromFile("../input - Copy.txt", list))
		{
			return WARNING;
		}

		// Check for real file.
		if (parseInputFromFile("../input.txt", list))
		{
			return GOOD;
		}

		std::cerr << "Couldn't find input file.\n";
		return BAD;
	}


	bool parseInputFromFile(const char* filename, std::vector<std::string>& list)
	{
		if (std::ifstream input{ filename }; input)
		{
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
		shared::InputResult result{ shared::parseInputFile(m_list) };

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
		std::string font_path{ shared::path + "LemonMilk/LemonMilkRegular-X3XE2.otf" };
		if (!m_font.loadFromFile(font_path))
		{
			printf("Could not load font from \"%s\"", font_path.c_str());
		}
		m_text.setFont(m_font);
		m_text.setCharacterSize(m_font_size);
		m_text.setFillColor(sf::Color::White); 
	}

	virtual bool Tick(float dt, bool can_advance) { return true; }

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


int run(Challenge& challenge, bool use_puzzle_size = false)
{
	shared::timer();
	
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
	unsigned int screen_width{ use_puzzle_size ? challenge.m_width * challenge.m_font_size : config::screen_width };
	unsigned int screen_height{ use_puzzle_size ? challenge.m_height * challenge.m_font_size : config::screen_height };
	sf::RenderWindow window(sf::VideoMode(screen_width, screen_height), "Challenge Begin!");
	//window.setKeyRepeatEnabled(false);
	challenge.m_window = &window;

	// Loop variables.
	bool skip_wait{ false };
	bool can_advance{ false };
	bool is_running{ true };
	bool is_done{ false };

	// Main loop.
	{
		sf::Clock clock;
		float delta_time{ 0.0f };

#if SKIP_VISUALIZE
		while (!challenge.Tick(0.0f, true));
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