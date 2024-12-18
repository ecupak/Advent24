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

// Priority queue.
#include <queue>

// For SFML when using rendering loop.
#include "SFML/Graphics.hpp"
#include <SFML/System/Clock.hpp>


namespace shared
{
	static std::string path{ "../../shared/" };

	enum InputResult
	{
		GOOD = 0,
		WARNING = 1,
		BAD = 2,
	};

	static void fillListWithInput(std::ifstream& input, std::vector<std::string>& list)
	{
		std::string strInput;

		while (input >> strInput)
		{
			list.push_back(strInput);
		}
		input.close();
	}


	static void fillListWithLines(std::ifstream& input, std::vector<std::string>& list)
	{
		while (!input.eof())
		{
			std::string value;
			std::getline(input, value, '\n');

			if (value.empty()) continue;

			list.push_back(value);
		}
	}


	static bool parseInputFromFile(const char* filename, std::vector<std::string>& list, bool as_lines)
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


	// 'as_lines' true will copy every input line as-is. If false, will copy individual values separated by space/linebreak.
	static InputResult parseInputFile(std::vector<std::string>& list, bool as_lines)
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

		
	// [Credit] https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c/22387757#22387757 + Alex
	static void timer(bool stop = false)
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
	static void getSpaceDelimitedValuesFromLine(const std::string& line, std::vector<T>& values)
	{
		std::istringstream full_line{ line };

		while (!full_line.eof())
		{
			T value;

			full_line >> value;

			values.push_back(value);
		}
	}

	template <typename T>
	struct Cell
	{
		Cell() = default;

		Cell(T _x, T _y)
			: x{ static_cast<T>(_x) }
			, y{ static_cast<T>(_y) }
		{	}

		T x{ 0 };
		T y{ 0 };

		Cell operator+(const Cell& rhs) const
		{
			return { this->x + rhs.x, this->y + rhs.y };
		}

		Cell operator-(const Cell& rhs) const
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

		bool operator!=(const Cell& other) const
		{
			return (x != other.x || y != other.y);
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

		friend Cell<T> operator*(const Cell& c, const float f);
	};

	template <typename T>
	Cell<T> operator*(const Cell<T>& c, const T f)
	{
		return { c.x * f, c.y * f };
	}

	template <typename T>
	Cell<T> operator*(const T f, const Cell<T>& c)
	{
		return { c.x * f, c.y * f };
	}

	template <typename T>
	Cell<T> directions[4]{
		// X   Y
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


	static unsigned int makeHash(int x, int y)
	{
		return x + (y << 16);
	}

	static void unhashMe(unsigned int hash, int& x, int& y)
	{
		x = hash & 0x0000FFFF;
		y = hash >> 16;
	}


	// [Credit] https://stackoverflow.com/questions/1489830/efficient-way-to-determine-number-of-digits-in-an-integer
	template <class T>
	static int numDigits(T number)
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
	static const int screen_width{ 600 };
	static const int screen_height{ 480 };

	static const int target_fps{ 60 };
	static const float target_fps_as_delta_time{ 1.0f / target_fps };

	static const int minimum_fps{ 10 };
	static const float maximum_delta_time{ 1.0f / minimum_fps };
} // namespace config


namespace vizit
{

class Challenge
{
public:
	// Gets input file and loads as vector of strings into m_list.
	int LoadFile()
	{
		shared::InputResult result{ shared::parseInputFile(m_list, m_load_as_line) };

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

	// Gets font to use and font size.
	// Override font size in derived Init() and call super to apply.	
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

	// Puzzle logic. Return true when completed/solved.
	virtual bool Tick(float dt) { return true; }

	// Rendering logic.
	virtual void Render(bool is_done = false) {}

	// Controls frequency/speed of Tick().
	bool IsReady(float dt)
	{
		if ((m_elapsed_time += dt) >= m_anim_delay)
		{
			m_elapsed_time = 0.0f;
			return true;
		}

		return false;
	}

	// Input loading. As line or separate all values.
	bool m_load_as_line{ true };

	// Animation speed.
	float m_anim_delay{ 0.01f };
	float m_elapsed_time{ 0.0f };

	// Rendering context.
	sf::RenderWindow* m_window;

	// Input parsed as a string. Final answer.
	std::vector<std::string> m_list;
	size_t m_total{ 0 };
	
	// # of letters per line and # of lines. Set during init, can be overwritten as needed.
	int m_width{ 0 };
	int m_height{ 0 };

	// Render strings using this.
	sf::Font m_font;
	sf::Text m_text;
	int m_font_size{ 24 };
};


static int run(Challenge& challenge, bool use_puzzle_size)
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
	while (!challenge.Tick(0.0f));	
#else
	bool skip_wait{ false };
	bool can_advance{ false };
	bool is_running{ true };
	bool is_done{ false };

	// Main loop.
	{
		sf::Clock clock;
		float delta_time{ 0.0f };
		float elapsed_time{ 0.0f };

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
				challenge.m_elapsed_time += delta_time;

				// Do puzzle logic.
				if (can_advance && (!skip_wait || challenge.IsReady(delta_time)))
				{					
					if (challenge.Tick(delta_time))
					{
						is_done = true;
					
						// If was advancing automatically, pause at end of execution.
						skip_wait = false;
					}

					// Update title with new total.
					std::string title{ "Total: " + std::to_string(challenge.m_total) };
					window.setTitle(title);
				}

				// Render puzzle.
				challenge.Render(is_done);

				// Reset advancement check.
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
	}
#endif

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
	static void createCombinations(std::vector<std::string>& results, std::string& given_str, char* data, uint64_t last, int index)
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
	static void get(std::vector<std::string>& results, std::string& given_str, size_t length = 0)
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


namespace container
{
	// Circular buffer. Once the end is reached, the next write operation will overwrite the element at index 0.
	// The head is always advanced after a write operation. Reading data with operator[] starts at the head position.
	// 
	// The index given when reading data is an offset to the head. In effect, this always means operator[0] returns the oldest entry in the buffer.
	// Negative indexing is supported. Thus operator[-1] will return the newest entry in the buffer.

	template <typename T>
	class RingBuffer
	{
	public:
		RingBuffer(size_t max_size)
			: m_capacity{ max_size }
		{
			m_buffer.resize(max_size);
		}


		// Called once to set the size of the internal vector.
		void initialize(size_t max_size)
		{
			if (m_capacity > 0) return;

			m_capacity = max_size;
			m_buffer.resize(max_size);
		}

		// Write data to the current head position. Advances head after writing.
		void write(const T& value);
		void write(const T&& value);

		// Get value from buffer at given index. Index is offset by current head index.
		T& operator[](const size_t index);
		const T& operator[](const size_t index) const;

		// Erases all buffer values.
		void clear();

		// Checks size value.
		bool isEmpty() const;

		// Returns size value.
		size_t size() const;

		// Custom iterator.
		struct Iterator
		{
		public:
			Iterator(const RingBuffer<T>* parent, size_t index, size_t cycle) noexcept;

			// Prefix increment.
			Iterator& operator++() noexcept;

			// Postfix increment.
			Iterator operator++(int) noexcept;

			// Inequality operator.		
			bool operator!=(const Iterator& other) const noexcept;

			// Get data from the slot.
			T operator*() const noexcept;

		private:
			// Needed to retrieve values from outer class.
			const RingBuffer<T>* m_parent;

			// For a ring buffer, we need to store the current index and the index of the cycle/loop.
			// The start and end are the same index in this situation, so the cycle index tells us if we've really reached the end or not.
			struct Slot
			{
				Slot(size_t index, size_t cycle)
					: m_index{ index }
					, m_cycle{ cycle }
				{	}

				friend bool operator==(const Slot& lhs, const Slot& rhs)
				{
					return (lhs.m_index == rhs.m_index) && (lhs.m_cycle == rhs.m_cycle);
				}

				friend bool operator!=(const Slot& lhs, const Slot& rhs)
				{
					return !(lhs == rhs);
				}

				size_t m_index{ 0 };
				size_t m_cycle{ 0 };
			};

			Slot m_slot{};
		};

		// Returns iterators to the start and end of the buffer.
		Iterator begin() const noexcept;
		Iterator end() const noexcept;

	private:
		// Increments head index. Wraps around to 0 when it is equal to capacity.
		// Size increments as well, until it is also equal to capacity.
		void AdvanceHead();

		std::vector<T> m_buffer;
		size_t m_head{ 0 };
		size_t m_size{ 0 };
		size_t m_capacity{ 0 };
	};


	template <typename T>
	void RingBuffer<T>::write(const T& value)
	{
		m_buffer[m_head] = value;

		AdvanceHead();
	}


	template <typename T>
	void RingBuffer<T>::write(const T&& value)
	{
		m_buffer[m_head] = value;

		AdvanceHead();
	}


	template <typename T>
	T& RingBuffer<T>::operator[](const size_t index)
	{
		size_t v{ (m_head + index) % m_size };
		return m_buffer[v];
	}


	template <typename T>
	const T& RingBuffer<T>::operator[](const size_t index) const
	{
		size_t v{ (m_head + index) % m_size };
		return m_buffer[v];
	}

	template <typename T>
	void RingBuffer<T>::clear()
	{
		m_buffer.clear();
		m_head = 0;
		m_size = 0;
	}


	template <typename T>
	bool RingBuffer<T>::isEmpty() const
	{
		return m_size == 0;
	}


	template <typename T>
	size_t RingBuffer<T>::size() const
	{
		return m_size;
	}


	template <typename T>
	typename RingBuffer<T>::Iterator RingBuffer<T>::begin() const noexcept
	{
		// If buffer is still being filled in, head index will equal size.
		// Begin iter will be 0 index on starting cycle.
		if (m_head == m_size) return Iterator{ this, 0, 0 };

		// Otherwise, begin iter is current index on starting cycle.
		return Iterator{ this, m_head, 0 };
	}


	template <typename T>
	typename RingBuffer<T>::Iterator RingBuffer<T>::end() const noexcept
	{
		// If no elements added, then end iter is same as begin iter.
		if (m_size == 0) return Iterator{ this, m_head, 0 };

		// If buffer is still being filled in, head index will equal size.
		// End iter will be 0 index on next cycle.
		if (m_head == m_size) return Iterator{ this, 0, 1 };

		// Otherwise, end iter is current index on next cycle.
		return Iterator{ this, m_head, 1 };
	}


	template <typename T>
	void RingBuffer<T>::AdvanceHead()
	{
		++m_head;

		if (m_size != m_capacity)
		{
			++m_size;
		}

		if (m_head == m_capacity)
		{
			m_head = 0;
		}
	}


	// Custom iterator.
	template <typename T>
	RingBuffer<T>::Iterator::Iterator(const RingBuffer<T>* parent, size_t index, size_t cycle) noexcept
		: m_parent{ parent }
		, m_slot{ index, cycle }
	{	}


	template <typename T>
	typename RingBuffer<T>::Iterator& RingBuffer<T>::Iterator::operator++() noexcept
	{
		++m_slot.m_index;

		if (m_slot.m_index == m_parent->m_size)
		{
			m_slot.m_index = 0;
			++m_slot.m_cycle;
		}

		return *this;
	}


	template <typename T>
	typename RingBuffer<T>::Iterator RingBuffer<T>::Iterator::operator++(int) noexcept
	{
		Iterator temp = *this;
		++*this;
		return temp;
	}


	template <typename T>
	bool RingBuffer<T>::Iterator::operator!=(const Iterator& other) const noexcept
	{
		return m_slot != other.m_slot;
	}


	template <typename T>
	T RingBuffer<T>::Iterator::operator*() const noexcept
	{
		return m_parent->m_buffer[m_slot.m_index];
	}


	template <typename TPriorityValue, typename TItemValue>
	class PairedPriorityQueue
	{
	public:
		PairedPriorityQueue() = default;

		void Put(const TItemValue item, const TPriorityValue priority) { m_elements.emplace(priority, item); }
		
		TItemValue Get()
		{
			TItemValue top_item{ m_elements.top().second };
			m_elements.pop();

			return top_item;
		}
		
		bool IsEmpty() { return m_elements.empty(); }

	private:
		using queue_element = std::pair<TPriorityValue, TItemValue>;
		std::priority_queue<queue_element, std::vector<queue_element>, std::greater<queue_element>> m_elements;
	};
}