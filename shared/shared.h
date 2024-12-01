#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <sstream>
#include <algorithm>


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

}// namespace shared