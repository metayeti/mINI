#include <iostream>
#include <string>
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
const T_INIFileData testDataBasic {
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

const T_INIFileData testDataWithGarbage {
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
		"ignored key = ignored value",
		"ignored=ignored",
		"GARBAGE",
		"",
		"ignored key2=ignored key2",
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
		""
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
		"[new section]",
		"test=something"
	}
};

const T_INIFileData testDataFormatted {
	// filename
	"data03.ini",
	// original data
	{
		"[ Food ]",
		"Cheese = 32",
		"Ice Cream = 64",
		"Bananas = 128",
		"",
		"[  Things  ]",
		"Scissors   = AAA",
		"Wooden Box = BBB",
		"Speakers   = CCC"
	},
	// expected result
	{
		"[ Food ]",
		"Cheese = AAA",
		"Ice Cream = BBB"
		"Bananas = CCC",
		"soup=DDD",
		"",
		"[  Things  ]",
		"Scissors   = 32",
		"Wooden Box = 64",
		"Speakers   = 128",
		"book=256"
	}
};

const T_INIFileData testDataRemEntry {
	// filename
	"data04.ini",
	// original data
	{
		"[section]",
		"data1=A",
		"data2=B"
	},
	// expected result
	{
		"[section]",
		"data2=B"
	}
};

const T_INIFileData testDataRemSection {
	// filename
	"data05.ini",
	// original data
	{
		"[section]",
		"data1=A",
		"data2=B"		
	},
	// expected result
	{
	}
};

const T_INIFileData testDataDuplicateKeys {
	// filename
	"data06.ini",
	// original data
	{
		"[section]",
		"data=A",
		"data=B",
		"[section]",
		"data=C"
	},
	// expected result
	{
		"[section]",
		"data=D",
		"data=D",
		"[section]",
		"data=D"
	}
};

const T_INIFileData testDataDuplicateSections {
	// filename
	"data07.ini",
	// original data
	{
		"[section]",
		"[section]",
		"[section]"
	},
	// expected result
	{
		"[section]",
		"data=A",
		"[section]",
		"data=A",
		"[section]",
		"data=A"
	}
};

const T_INIFileData testDataPrettyPrint {
	// filename
	"data08.ini",
	// oiriginal data
	{
		"[section1]",
		"value1=A",
		"value2=B",
		"[section2]",
		"value1=A"
	},
	// expected result
	{
		"[section1]",
		"value1=A",
		"value2=B",
		"value3 = C",
		"[section2]",
		"value1=A",
		"value2 = B",
		"",
		"[section3]",
		"value1 = A",
		"value2 = B"
	}
};

const T_INIFileData testDataEmptyFile {
	// filename
	"data09.ini",
	// original data
	{},
	// expected results
	{
		"[section]",
		"key=value"
	}
};

const T_INIFileData testDataEmptySection {
	// filename
	"data10.ini",
	// original data
	{
		"[section]"
	},
	// expected result
	{
		"[section]",
		"key=value"
	}
};

const T_INIFileData testDataManyEmptySections {
	// filename
	"data11.ini",
	// original data
	{
		"[section1]",
		"[section2]",
		"[section3]",
		"[section4]",
		"[section5]"
	},
	// expected result
	{
		"[section1]",
		"[section2]",
		"[section3]",
		"key=value",
		"[section4]",
		"[section5]"
	}
};

//
// test cases
//
const lest::test mINI_tests[] = {
	CASE("TEST: Basic write")
	{
		// do some basic modifications to an INI file
		// then compare resulting file to expected data
		std::string const& filename = std::get<0>(testDataBasic);
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		ini["some section"]["some key"] = "2";
		ini["some section"]["yet another key"] = "3";
		EXPECT(file.write(ini) == true);
		EXPECT(verifyData(testDataBasic));
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
		ini["new section"]["test"] = "something";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataWithGarbage));
	},
	CASE("Test: Remove entry")
	{
		auto const& filename = std::get<0>(testDataRemEntry);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"].remove("data1");
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataRemEntry));
	},
	CASE("Test: Remove section")
	{
		auto const& filename = std::get<0>(testDataRemSection);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini.remove("section");
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataRemSection));
	},
	CASE("Test: Duplicate keys")
	{
		auto const& filename = std::get<0>(testDataDuplicateKeys);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["data"] = "D";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataDuplicateKeys));
	},
	CASE("Test: Duplicate sections")
	{
		auto const& filename = std::get<0>(testDataDuplicateSections);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["data"] = "A";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataDuplicateSections));
	},
	CASE("Test: Pretty print")
	{
		auto const& filename = std::get<0>(testDataPrettyPrint);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section1"]["value3"] = "C";
		ini["section2"]["value2"] = "B";
		ini["section3"].set({
			{"value1", "A"},
			{"value2", "B"}
		});
		// write to file
		EXPECT(file.write(ini, true) == true);
		// verify data
		EXPECT(verifyData(testDataPrettyPrint));
	},
	CASE("Test: Write to empty file")
	{
		auto const& filename = std::get<0>(testDataEmptyFile);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["key"] = "value";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataEmptyFile));
	},
	CASE("Test: Write to empty section")
	{
		auto const& filename = std::get<0>(testDataEmptySection);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section"]["key"] = "value";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataEmptySection));
	},
	CASE("Test: Write to a single empty section among many")
	{
		auto const& filename = std::get<0>(testDataManyEmptySections);
		// read from file
		mINI::INIFile file(filename);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		// update data
		ini["section3"]["key"] = "value";
		// write to file
		EXPECT(file.write(ini) == true);
		// verify data
		EXPECT(verifyData(testDataManyEmptySections));
	}
};

int main(int argc, char** argv)
{
	// write test files
	writeTestFile(testDataBasic);
	writeTestFile(testDataWithGarbage);
	writeTestFile(testDataRemEntry);
	writeTestFile(testDataRemSection);
	writeTestFile(testDataDuplicateKeys);
	writeTestFile(testDataDuplicateSections);
	writeTestFile(testDataPrettyPrint);
	writeTestFile(testDataEmptyFile);
	writeTestFile(testDataEmptySection);
	writeTestFile(testDataManyEmptySections);

	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}
