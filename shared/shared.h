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

	const int parseInputFile(std::vector<std::string>& list)
	{
		std::ifstream inf{ "../input.txt" };

		if (!inf)
		{
			std::cerr << "Couldn't find file.\n";
			return 1;
		}

		std::string strInput;

		while (inf)
		{
			std::getline(inf, strInput);
			list.push_back(strInput);
		}
		inf.close();

		return 0;
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


	// [Credit] https://stackoverflow.com/questions/22387586/measuring-execution-time-of-a-function-in-c/22387757#22387757 + Alex
	void timer(bool stop = false)
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

}// namespace shared