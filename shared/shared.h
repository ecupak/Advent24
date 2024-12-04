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


namespace shared
{
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

}// namespace shared