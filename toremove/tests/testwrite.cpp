#include <vector>
#include <string>
#include <fstream>
#include "lest.hpp"
#include "mini/ini.h"

typedef std::vector<std::string> T_LineData;

//
// test data: original files and expected results after updates
//

// basic ini file
const T_LineData data_INI_basic_initial = {
	"; this is a comment",
	"[section]",
	"key1=value1",
	"key2=value2"
};

const T_LineData data_INI_basic_expected = {
	"; this is a comment",
	"[section]",
	"key1=newvalue1",
	"key2=newvalue2"
};

// complex ini file
const T_LineData data_INI_complex_initial = {
	"   GARBAGE   ",
	";;;;;;;;;;;;;;comment comment",
	";;;;;;",
	";",
	"GARBAGE",
	"GARBAGE  ",
	"[section1] ; trailing comment",
	"GARBAGE",
	";;",
	"a=1",
	"bbb = 3",
	"ccc          = 4",
	"ddddd  =       5",
	";",
	";",
	"",
	"[other section]",
	"dinosaurs= 16",
	"birds= 123.456",
	"giraffes= 2",
	"[extra section]",
	"            a = 1",
	"            b=2",
	"            c    =    3",
	";end of original file"
};

const T_LineData data_INI_complex_expected = {
	"   GARBAGE   ",
	";;;;;;;;;;;;;;comment comment",
	";;;;;;",
	";",
	"GARBAGE",
	"GARBAGE  ",
	"[section1] ; trailing comment",
	"GARBAGE",
	";;",
	"a=2",
	"bbb = 100",
	"ccc          = -500",
	"ddddd  =       0xFF",
	"newkey=newvalue",
	";",
	";",
	"",
	"[other section]",
	"dinosaurs= 16",
	"birds= all",
	"giraffes= 50",
	"elephants=200",
	"lions=64",
	"[extra section]",
	"            a = -1",
	"            b=3",
	"            c    =    0",
	";end of original file",
	"[new section]",
	"key1=value1",
	"key2=value2"
};

// toupdate ini file
const T_LineData data_INI_toupdate_initial = {
	"[colors]",
	"Red=255",
	"GREEN=0",
	"blue=100"
	"[toremovenone]",
	"[toremoveone]",
	"A=B",
	"[toremovefull]",
	"key1=value1",
	"key2=value2",
	"key3=value3",
	"[toupdate]",
	"original=100",
	"modified=100"
};

const T_LineData data_INI_toupdate_expected = {
};

//
// filenames
//

const std::string filename_INI_basic = "data01.ini";
const std::string filename_INI_complex = "data02.ini";

//
// helper functions
//

bool WriteTestINI(std::string const& filename, T_LineData const& lines)
{
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

bool VerifyData(std::string const& filename, T_LineData const& lines)
{
	// verifies if contents in file match given line data
	std::ifstream fileReadStream(filename);
	std::string line;
	size_t lineCount = 0;
	size_t lineCountExpected = lines.size();
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
			std::string const& lineExpected = lines[lineCount++];
			if (line != lineExpected)
			{
				std::cout << "Line " << lineCount << " does not match expected!" << std::endl;
				std::cout << "Should be: " << lineExpected << std::endl;
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
// test cases
//
const lest::test mINI_tests[] =
{
	CASE("Basic write")
	{
		// change two keys from input file and check for correctness
		mINI::INIFile iniDataBasic(filename_INI_basic);
		iniDataBasic.Set("section", "key1", "newvalue1");
		iniDataBasic.Set("section", "key2", "newvalue2");
		EXPECT(iniDataBasic.Write() == true);
		EXPECT(VerifyData(filename_INI_basic, data_INI_basic_expected) == true);
	},
	
	CASE("Complex write")
	{
		// change a few keys from input file and add a new section,
		// then check for correctness
		mINI::INIFile iniDataComplex(filename_INI_complex);
		iniDataComplex.Set("section1", {
			{"a", "2"},
			{"bbb", "100"},
			{"ccc", "-500"},
			{"ddddd", "0xFF"},
			{"newkey", "newvalue"}
		});
		iniDataComplex.Set("other section", {
			{"birds", "all"},
			{"giraffes", "50"},
			{"elephants", "200"},
			{"lions", "64"}
		});
		iniDataComplex.Set("extra section", {
			{"     a", "     -1"},
			{"b     ", "3      "},
			{"    c  ", "   0  "}
		});
		iniDataComplex.Set("new section", {
			{"key1", "value1"},
			{"key2", "value2"}
		});
		EXPECT(iniDataComplex.Write() == true);
		EXPECT(VerifyData(filename_INI_complex, data_INI_complex_expected) == true);
	}
};

int main(int argc, char** argv)
{
	// write test files
	WriteTestINI(filename_INI_basic, data_INI_basic_initial);
	WriteTestINI(filename_INI_complex, data_INI_complex_initial);
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}

	return std::cout << std::endl << "All tests passed!" << std::endl, EXIT_SUCCESS;
}
