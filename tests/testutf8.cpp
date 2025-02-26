#include <iostream>
#include <string>
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

bool writeTestFile(T_INIFileData const& testData)
{
	std::string const& filename = testData.first;
	T_LineData const& lines = testData.second;
	std::ofstream fileWriteStream(filename);
	if (fileWriteStream.is_open())
	{
		const char utf8_BOM[3] = {
			static_cast<char>(0xEF),
			static_cast<char>(0xBB),
			static_cast<char>(0xBF)
		};
		fileWriteStream.write(utf8_BOM, 3);

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

//
// test data
//
const T_INIFileData testDataUTF8BOM = {
	// filename
	"utf8bom.ini",
	// test data
	{
		"[section]",
		"key=value",
		"key2=value2",
		"[section2]",
		"key=value"
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("Test: Write and read back utf-8 values")
	{
		mINI::INIFile file("data01.ini");
		mINI::INIStructure ini;
		ini["section"]["key"] = "€";
		ini["section"]["€"] = "value";
		ini["€"]["key"] = "value";
		ini["section"]["key2"] = "𐍈";
		ini["section"]["𐍈"] = "value";
		ini["𐍈"]["key"] = "value";
		ini["section"]["key3"] = "你好";
		ini["section"]["你好"] = "value";
		ini["你好"]["key"] = "value";
		EXPECT(file.write(ini) == true);
		ini.clear();
		EXPECT(file.read(ini) == true);
		outputData(ini);
		EXPECT(ini["section"]["key"] == "€");
		EXPECT(ini["section"]["key2"] == "𐍈");
		EXPECT(ini["section"]["€"] == "value");
		EXPECT(ini["€"]["key"] == "value");
		EXPECT(ini["section"]["𐍈"] == "value");
		EXPECT(ini["𐍈"]["key"] == "value");
		EXPECT(ini["section"]["key3"] == "你好");
		EXPECT(ini["section"]["你好"] == "value");
		EXPECT(ini["你好"]["key"] == "value");
	},
	CASE("Test: UTF-8 BOM encoded file")
	{
		const auto& filename = testDataUTF8BOM.first;
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini["section"]["key"] == "value");
		EXPECT(ini["section"]["key2"] == "value2");
		EXPECT(ini["section2"]["key"] == "value");
		// update
		ini["section"]["key"] = "something else";
		// write
		EXPECT(file.write(ini) == true);
		// expect BOM encoding
		mINI::INIReader testReader(filename);
		mINI::INIStructure testStructure;
		testReader >> testStructure;
		EXPECT(testReader.isBOM == true);
		// verify data
		EXPECT(testStructure["section"]["key"] == "something else");
	}
};

int main(int argc, char** argv)
{
	// write test files
	writeTestFile(testDataUTF8BOM);
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}

