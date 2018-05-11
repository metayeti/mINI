#include <iostream>
#include <vector>
#include <tuple>
#include <fstream>
#include "lest.hpp"
#include "mini/ini.h"

using T_LineData = std::vector<std::string>;
using T_INIFileData = std::tuple<std::string, T_LineData, T_LineData>;

//
// helper functions
//
bool writeTestFile(T_INIFileData const& testData)
{
	std::string const& filename = std::get<0>(testData);
	T_LineData const& lines = std::get<1>(testData);
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
				fileWriteStream << std::endl;
			}
		}
		return true;
	}
	return false;
}

bool verifyData(T_INIFileData const& testData)
{
	// compares file contents to expected data
	std::string line;
	std::string const& filename = std::get<0>(testData);
	T_LineData const& linesExpected = std::get<2>(testData);
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

void outputData(mINI::INIStructure const& ini)
{
	for (auto const& it : ini)
	{
		auto const& section = it.first;
		auto const& collection = it.second;
		std::cout << "[" << section << "]" << std::endl;
		for (auto const& it2 : collection)
		{
			auto const& key = it2.first;
			auto const& value = it2.second;
			std::cout << key << "=" << value << std::endl;
		}
		std::cout << std::endl;
	}
}

//
// test data
//
T_INIFileData testDataBasic = {
	// filename
	"data01.ini",
	// original data
	{
		";some comment",
		"[some section]",
		"some key=1",
		"other key=2"
	},
	// expected result
	{
		";some comment",
		"[some section]",
		"some key=2",
		"other key=2",
		"yet another key=3"
	}
};

T_INIFileData testDataWithGarbage = {
	// filename
	"data02.ini",
	// original data
	{
		"",
		"  GARBAGE       ; ALSO GARBAGE ",
		";;;;;;;;;;;;;;;comment comment",
		";;;;",
		";;;;       ",
		"          ;",
		"         ;;      ;;xxxx       ",
		"GARBAGE",
		"",
		"GARBAGE             ;;;;;;;;;;;;;;;;;;;;;",
		"[section] ; trailing comment",
		"",
		"GARBAGE",
		";;",
		"a=1",
		"b = 2",
		"c =3",
		"d= 4",
		"e =  5",
		"f   =6",
		"",
		"@#%$@(*(!@*@GARBAGE#!@GARBAGE%$@#GARBAGE%@&*%@$",
		"GARBAGE",
		"[other section] ;also a comment",
		"GARBAGE",
		"dinosaurs= 16",
		"GARBAGE",
		"birds= 123456",
		"giraffes= 22",
		"GARBAGE",
		"[extra section];also a comment",
		"         aaa = 1",
		"         bbb=2",
		"         ccc  =  3",
		"GARBAGE",
		"",
		"",
		";eof"
	},
	// expected result
	{
		"",
		";;;;;;;;;;;;;;;comment comment",
		";;;;",
		";;;;       ",
		"          ;",
		"         ;;      ;;xxxx       ",
		"",
		"[section] ; trailing comment",
		"",
		";;",
		"a=2",
		"b = 3",
		"c =4",
		"d= 5",
		"e =  6",
		"f   =7",
		"g=8",
		"",
		"[other section] ;also a comment",
		"birds= 123456",
		"giraffes= 22",
		"[extra section];also a comment",
		"         aaa = 2",
		"         bbb=3",
		"         ccc  =  4",
		"ddd=5",
		"",
		"",
		";eof"
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("TEST: Basic write")
	{
		// do some basic mofidications to an INI file
		// then compare resulting file to expected data
		std::string const& filename = std::get<0>(testDataBasic);
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		ini["some section"]["some key"] = "2";
		ini["some section"]["yet another key"] = "3";
		EXPECT(file.write(ini) == true);
		EXPECT(verifyData(testDataBasic) == true);
	},
	CASE("TEST: Garbage data")
	{
		auto const& filename = std::get<0>(testDataWithGarbage);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"].set({
			{"a", "2"},
			{"b", "3"},
			{"c", "4"},
			{"d", "5"},
			{"e", "6"},
			{"f", "7"},
			{"g", "8"}
		});
		ini["other section"].remove("dinosaurs"); // sorry, dinosaurs
		ini["extra section"].set({
			{"aaa", "2"},
			{"bbb", "3"},
			{"ccc", "4"},
			{"ddd", "5"}
		});
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataWithGarbage));
	}
};

int main(int argc, char** argv)
{
	// write test files
	writeTestFile(testDataBasic);
	writeTestFile(testDataWithGarbage);
	
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}
