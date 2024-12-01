#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>


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

}// namespace shared