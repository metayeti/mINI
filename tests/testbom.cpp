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

bool writeBOMTestFile(std::string const& filename, T_LineData const& lines)
{
	std::ofstream fileWriteStream(filename, std::ios::out | std::ios::binary);
	if (!fileWriteStream.is_open())
	{
		return false;
	}

	const char utf8_BOM[3] = {
		static_cast<char>(0xEF),
		static_cast<char>(0xBB),
		static_cast<char>(0xBF)
	};

	fileWriteStream.write(utf8_BOM, 3);
	for (auto it = lines.begin(); it != lines.end(); ++it)
	{
		fileWriteStream << *it;
		if (std::next(it) != lines.end())
		{
			fileWriteStream << std::endl;
		}
	}
	return true;
}

bool checkIfFileIsBOM(std::string const& filename)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file.is_open()) return false;
	
	char header[3];
	file.read(header, 3);
	
	return (header[0] == static_cast<char>(0xEF) &&
			header[1] == static_cast<char>(0xBB) &&
			header[2] == static_cast<char>(0xBF));
}

//
// test data
//

const T_INIFileData testDataBOM = {
	"testbom.ini",
	{
		"[data]",
		"test=XYZ"
	}
};

const T_INIFileData testDataBOMMultiple = {
	"testbom_multi.ini",
	{
		"[section1]",
		"key1=value1",
		"key2=value2",
		"[section2]",
		"keyA=valueA",
		"keyB=valueB"
	}
};

const lest::test mINI_tests[] = {
	CASE("Test: BOM file read with data integrity check")
	{
		EXPECT(writeBOMTestFile(testDataBOM.first, testDataBOM.second) == true);

		mINI::INIFile file(testDataBOM.first);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		EXPECT(ini["data"]["test"] == "XYZ");
	},
	CASE("Test: BOM detection on read")
	{
		EXPECT(writeBOMTestFile(testDataBOM.first, testDataBOM.second) == true);

		mINI::INIReader reader(testDataBOM.first);
		mINI::INIStructure ini;
		EXPECT((reader >> ini));
		EXPECT(reader.isBOM == true);
	},
	CASE("Test: BOM preserved on write")
	{
		EXPECT(writeBOMTestFile(testDataBOM.first, testDataBOM.second) == true);

		// read, modify, and write back
		mINI::INIFile file(testDataBOM.first);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		
		ini["data"]["test"] = "modified";
		EXPECT(file.write(ini) == true);

		// check that BOM is still present in file
		EXPECT(checkIfFileIsBOM(testDataBOM.first) == true);

		// verify data is correct
		ini.clear();
		EXPECT(file.read(ini) == true);
		EXPECT(ini["data"]["test"] == "modified");
	},
	CASE("Test: BOM with multiple sections and keys")
	{
		EXPECT(writeBOMTestFile(testDataBOMMultiple.first, testDataBOMMultiple.second) == true);

		mINI::INIFile file(testDataBOMMultiple.first);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		
		EXPECT(ini["section1"]["key1"] == "value1");
		EXPECT(ini["section1"]["key2"] == "value2");
		EXPECT(ini["section2"]["keyA"] == "valueA");
		EXPECT(ini["section2"]["keyB"] == "valueB");
	},
	CASE("Test: BOM preserved after modifying multiple values")
	{
		EXPECT(writeBOMTestFile(testDataBOMMultiple.first, testDataBOMMultiple.second) == true);

		mINI::INIFile file(testDataBOMMultiple.first);
		mINI::INIStructure ini;
		EXPECT(file.read(ini) == true);
		
		// Modify multiple values
		ini["section1"]["key1"] = "newValue1";
		ini["section2"]["keyA"] = "newValueA";
		ini["section1"]["key3"] = "addedKey";  // Add new key
		
		EXPECT(file.write(ini) == true);
		EXPECT(checkIfFileIsBOM(testDataBOMMultiple.first) == true);

		// Verify modifications persisted
		ini.clear();
		EXPECT(file.read(ini) == true);
		EXPECT(ini["section1"]["key1"] == "newValue1");
		EXPECT(ini["section2"]["keyA"] == "newValueA");
		EXPECT(ini["section1"]["key3"] == "addedKey");
		EXPECT(ini["section1"]["key2"] == "value2");  // Unchanged
	},
	CASE("Test: Generating new file from BOM-read structure preserves data")
	{
		EXPECT(writeBOMTestFile(testDataBOM.first, testDataBOM.second) == true);

		// read BOM file
		mINI::INIFile sourceFile(testDataBOM.first);
		mINI::INIStructure ini;
		EXPECT(sourceFile.read(ini) == true);
		
		// generate new file (should _not_ preserve BOM)
		mINI::INIFile newFile("testbom_generated.ini");
		EXPECT(newFile.generate(ini, false) == true);
		
		// verify data is readable
		ini.clear();
		EXPECT(newFile.read(ini) == true);
		EXPECT(ini["data"]["test"] == "XYZ");
	}
};

int main(int argc, char** argv)
{
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}
	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}