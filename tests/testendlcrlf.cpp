#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <fstream>
#include "lest.hpp"
#define MINI_ENDL_CRLF
#include "mini/ini.h"

using T_LineData = std::vector<std::string>;
using T_INIFileData = std::pair<std::string, T_LineData>;

const char* const CRLF = "\r\n";

//
// helper functions
//
bool writeTestFile(T_INIFileData const& testData)
{
	std::string const& filename = testData.first;
	T_LineData const& lines = testData.second;
	std::ofstream fileWriteStream(filename);
	if (fileWriteStream.is_open())
	{
		if (lines.size())
		{
			auto it = lines.begin();
			for (;;)
			{
				fileWriteStream << *it;
				if (++it == lines.end())
				{
					break;
				}
				fileWriteStream << CRLF;
			}
		}
		return true;
	}
	return false;
}

void outputData(mINI::INIStructure const& ini)
{
	for (auto const& it : ini)
	{
		auto const& section = it.first;
		auto const& collection = it.second;
		std::cout << "[" << section << "]" << CRLF;
		for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			std::cout << key << "=" << value << CRLF;
		}
		std::cout << CRLF;
	}
}

//
// test data
//
const T_INIFileData testDataBasic = {
	// filename
	"data01.ini",
	// test data
	{
		"[section]",
		"key = value",
	}
};

const lest::test mINI_tests[] = {
	CASE("Test: Read case sensitive")
	{
		// read a basic INI file and check if values are read correctly
		auto const& filename = testDataBasic.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		std::cout << filename << std::endl;
		outputData(ini);
		EXPECT(ini["section"]["key"] == "value");
	},
};

int main(int argc, char** argv)
{
	// write test files
	writeTestFile(testDataBasic);

	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}