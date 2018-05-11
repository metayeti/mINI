#include <iostream>
#include <vector>
#include <utility>
#include <fstream>
#include "lest.hpp"
#include "mini/ini.h"

using T_LineData = std::vector<std::string>;
using T_INIFileData = std::pair<std::string, T_LineData>;

//
// helper functions
//
bool verifyData(T_INIFileData const& testData)
{
	// compares file contents to expected data
	std::string line;
	std::string const& filename = testData.first;
	T_LineData const& linesExpected = testData.second;
	size_t lineCount = 0;
	size_t lineCountExpected = linesExpected.size();
	std::ifstream fileReadStream(filename);
	if (fileReadStream.is_open())
	{
		while (std::getline(fileReadStream, line))
		{
			if (fileReadStream.bad())
			{
				return false;
			}
			if (lineCount >= lineCountExpected)
			{
				std::cout << "Line count exceeds expected!" << std::endl;
				return false;
			}
			std::string const& lineExpected = linesExpected[lineCount++];
			if (line != lineExpected)
			{
				std::cout << "Line " << lineCount << " does not match expected!" << std::endl;
				std::cout << "Expected: " << lineExpected << std::endl;
				std::cout << "Is: " << line << std::endl;
				return false;
			}
		}
		if (lineCount < lineCountExpected)
		{
			std::cout << "Line count falls behind expected!" << std::endl;
		}
		return lineCount == lineCountExpected;
	}
	return false;
}

//
// test data
//
const T_INIFileData testDataBasic = {
	// filename
	"data01.ini",
	// expected result
	{
		"[section]",
		"key1=value1",
		"key2=value2"
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("TEST: Basic generate")
	{
		// create a very basic INI file and verify resulting file has correct data
		std::string const& filename = testDataBasic.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		ini["section"]["key1"] = "value1";
		ini["section"]["key2"] = "value2";
		EXPECT(file.write(ini) == true);
		EXPECT(verifyData(testDataBasic) == true);
	}
};

int main(int argc, char** argv)
{
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}

