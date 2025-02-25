#include <iostream>
#include <filesystem>
#include <string>
#include <vector>
#include <tuple>
#include <fstream>
#include "lest.hpp"
#include "mini/ini.h"

using T_LineData = std::vector<std::string>;
using T_INIFileData = std::tuple<std::filesystem::path, T_LineData, T_LineData>;

//
// helper functions
//
bool writeTestFile(T_INIFileData const& testData)
{
	auto const& filename = std::get<0>(testData);
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
	auto const& filename = std::get<0>(testData);
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
const T_INIFileData testDataStdString {
	// filename
	"data_stdstring.ini",
	// original data
	{
		"[fruit]",
		"bananas=1",
		"apples=2",
		"grapes=3",
	},
	// expected result
	{
		"[fruit]",
		"bananas=2",
		"apples=3",
		"grapes=4",
	}
};

const T_INIFileData testDataUTF16JP {
	// filename
	u"data_u16テスト.ini",
	// original data
	{
		"[fruit]",
		"bananas=1",
		"apples=2",
		"grapes=3",
	},
	// expected result
	{
		"[fruit]",
		"bananas=2",
		"apples=3",
		"grapes=4",
	}
};

const T_INIFileData testDataUTF16TC {
	// filename
	u"data_u16測試.ini",
	// original data
	{
		"[fruit]",
		"bananas=1",
		"apples=2",
		"grapes=3",
	},
	// expected result
	{
		"[fruit]",
		"bananas=2",
		"apples=3",
		"grapes=4",
	}
};

const T_INIFileData testDataUTF32JP {
	// filename
	U"data_u32テスト.ini",
	// original data
	{
		"[fruit]",
		"bananas=1",
		"apples=2",
		"grapes=3",
	},
	// expected result
	{
		"[fruit]",
		"bananas=2",
		"apples=3",
		"grapes=4",
	}
};

const T_INIFileData testDataUTF32TC {
	// filename
	U"data_u32測試.ini",
	// original data
	{
		"[fruit]",
		"bananas=1",
		"apples=2",
		"grapes=3",
	},
	// expected result
	{
		"[fruit]",
		"bananas=2",
		"apples=3",
		"grapes=4",
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("TEST: std::string read/write")
	{
		// read a INI file with std::string and check if values are read correctly
		auto const& filename = std::get<0>(testDataStdString);
		mINI::INIFile file(filename.string());
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini["fruit"]["bananas"] == "1");
		EXPECT(ini["fruit"]["apples"] == "2");
		EXPECT(ini["fruit"]["grapes"] == "3");
		// update data
		ini["fruit"]["bananas"] = "2";
		ini["fruit"]["apples"] = "3";
		ini["fruit"]["grapes"] = "4";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataStdString));
	},
	CASE("TEST: utf-16 jp read/write")
	{
		// read a INI file with utf-16 jp path and check if values are read correctly
		auto const& filename = std::get<0>(testDataUTF16JP);
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini["fruit"]["bananas"] == "1");
		EXPECT(ini["fruit"]["apples"] == "2");
		EXPECT(ini["fruit"]["grapes"] == "3");
		// update data
		ini["fruit"]["bananas"] = "2";
		ini["fruit"]["apples"] = "3";
		ini["fruit"]["grapes"] = "4";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataUTF16JP));
	},
	CASE("TEST: utf-16 tc read/write")
	{
		// read a INI file with utf-16 tc path and check if values are read correctly
		auto const& filename = std::get<0>(testDataUTF16TC);
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini["fruit"]["bananas"] == "1");
		EXPECT(ini["fruit"]["apples"] == "2");
		EXPECT(ini["fruit"]["grapes"] == "3");
		// update data
		ini["fruit"]["bananas"] = "2";
		ini["fruit"]["apples"] = "3";
		ini["fruit"]["grapes"] = "4";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataUTF16TC));
	},
	CASE("TEST: utf-32 jp read/write")
	{
		// read a INI file with utf-32 jp path and check if values are read correctly
		auto const& filename = std::get<0>(testDataUTF32JP);
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini["fruit"]["bananas"] == "1");
		EXPECT(ini["fruit"]["apples"] == "2");
		EXPECT(ini["fruit"]["grapes"] == "3");
		// update data
		ini["fruit"]["bananas"] = "2";
		ini["fruit"]["apples"] = "3";
		ini["fruit"]["grapes"] = "4";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataUTF32JP));
	},
	CASE("TEST: utf-32 tc read/write")
	{
		// read a INI file with utf-32 tc path and check if values are read correctly
		auto const& filename = std::get<0>(testDataUTF32TC);
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini["fruit"]["bananas"] == "1");
		EXPECT(ini["fruit"]["apples"] == "2");
		EXPECT(ini["fruit"]["grapes"] == "3");
		// update data
		ini["fruit"]["bananas"] = "2";
		ini["fruit"]["apples"] = "3";
		ini["fruit"]["grapes"] = "4";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataUTF32TC));
	},
};

int main(int argc, char** argv)
{
	// write test files
	writeTestFile(testDataStdString);
	writeTestFile(testDataUTF16JP);
	writeTestFile(testDataUTF16TC);
	writeTestFile(testDataUTF32JP);
	writeTestFile(testDataUTF32TC);
	
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}
