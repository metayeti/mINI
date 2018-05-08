#include <vector>
#include <string>
#include <fstream>
#include "lest.hpp"
#include "mini/ini.h"

typedef std::vector<std::string> T_LineData;

//
// test data: expected results
//

// basic generated file
const T_LineData data_INI_basic = {
	"[section]",
	"key1=value1",
	"key2=value2"
};

// empty file
const T_LineData data_INI_empty = {};

// generated file with types
const T_LineData data_INI_types = {
	"[string]",
	"str1=test string",
	"str2=another test string",
	"str3=UPPERCASE VALUE",
	"[bool]",
	"bool1=yes",
	"bool2=no",
	"[char]",
	"char1=c",
	"char2=C",
	"char3=?",
	"[int]",
	"int1=0",
	"int2=1000",
	"int3=-1000",
	"min=-2147483648",
	"max=2147483647",
	"[uint]",
	"uint1=0",
	"uint2=1000",
	"max=4294967295",
	"[double]",
	"double1=0",
	"double2=0.1",
	"double3=0.3",
	"double4=0.123456789123"
};

// pretty-printed generated file
const T_LineData data_INI_pretty = {
	"[section1]",
	"key1 = value1",
	"key2 = value2",
	"key3 = value3",
	"",
	"[section2]",
	"key1 = value1",
	"key2 = value2",
	"key3 = value3"
};

// edge case: single empty section
const T_LineData data_INI_edge_case_1 = {
	"[test]"
};

// edge case: many empty sections
const T_LineData data_INI_edge_case_2 = {
	"[test1]",
	"[test2]",
	"[test3]"
};

// edge case: many empty sections with non-empty sections in between
const T_LineData data_INI_edge_case_3 = {
	"[test1]",
	"[test2]",
	"[realdata]",
	"value1=A",
	"value2=B",
	"value3=C",
	"[test3]"
};

//
// filenames
//

const std::string filename_INI_basic = "data01.ini";
const std::string filename_INI_empty = "data02.ini";
const std::string filename_INI_types = "data03.ini";
const std::string filename_INI_pretty = "data04.ini";
const std::string filename_INI_edge_case_1 = "data05.ini";
const std::string filename_INI_edge_case_2 = "data06.ini";
const std::string filename_INI_edge_case_3 = "data07.ini";

const std::string filename_invalid = "?<>||*/\\..^^&&";

//
// helper functions
//

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
	CASE("Basic generate")
	{
		mINI::INIFile iniDataBasic;
		iniDataBasic.Set("section", "key1", "value1");
		iniDataBasic.Set("section", "key2", "value2");
		EXPECT(iniDataBasic.Generate(filename_INI_basic) == true);
		EXPECT(VerifyData(filename_INI_basic, data_INI_basic) == true);
	},

	CASE ("Generate empty")
	{
		mINI::INIFile iniDataEmpty;
		EXPECT(iniDataEmpty.Generate(filename_INI_empty) == true);
		EXPECT(VerifyData(filename_INI_empty, data_INI_empty) == true);
	},

	CASE ("Generate illegal filename")
	{
		mINI::INIFile iniDataIllegalFile;
		EXPECT(iniDataIllegalFile.Generate(filename_invalid) == false);
	},

	CASE ("Generate with types")
	{
		mINI::INIFile iniDataTypes;
		iniDataTypes.Set("string", "str1", "test string");
		iniDataTypes.Set("    STRING     ", "       STR2      ", "         another test string      ");
		iniDataTypes.Set("string", "str3", "UPPERCASE VALUE");
		iniDataTypes.Set("bool", "bool1", true);
		iniDataTypes.Set("bool", "bool2", false);
		iniDataTypes.Set("char", "char1", 'c');
		iniDataTypes.Set("char", "char2", 'C');
		iniDataTypes.Set("char", "char3", '?');
		iniDataTypes.Set("int", "int1", 0);
		iniDataTypes.Set("int", "int2", 1000);
		iniDataTypes.Set("int", "int3", -1000);
		iniDataTypes.Set("int", "min", int(-2147483648));
		iniDataTypes.Set("int", "max", 2147483647);
		iniDataTypes.Set("uint", "uint1", 0u);
		iniDataTypes.Set("uint", "uint2", 1000u);
		iniDataTypes.Set("uint", "max", 4294967295u);
		iniDataTypes.Set("double", "double1", 0.0);
		iniDataTypes.Set("double", "double2", .1);
		iniDataTypes.Set("double", "double3", .3);
		iniDataTypes.Set("double", "double4", .123456789123);
		EXPECT(iniDataTypes.Generate(filename_INI_types) == true);
		EXPECT(VerifyData(filename_INI_types, data_INI_types) == true);
	},

	CASE ("Generate pretty")
	{
		mINI::INIFile iniDataPretty;
		iniDataPretty.Set("section1", "key1", "value1");
		iniDataPretty.Set("section1", "key2", "value2");
		iniDataPretty.Set("section1", "key3", "value3");
		// test fancy set
		iniDataPretty.Set("section2", {
			{"key1", "value1"},
			{"key2", "value2"},
			{"key3", "value3"}
		});
		EXPECT(iniDataPretty.Generate(filename_INI_pretty, true) == true);
		EXPECT(VerifyData(filename_INI_pretty, data_INI_pretty) == true);
	},

	CASE ("Generate edge cases")
	{
		// edge case 1: single empty section
		mINI::INIFile iniEdgeCase1;
		// in order to add an empty section, we create a dummy value and remove the
		// key. the API doesn't provide a function to add empty sections on purpose
		// since it attempts to abstract the interface and let the user deal with
		// data and not the file structure
		iniEdgeCase1.Set("test", "dummy", "");
		iniEdgeCase1.Remove("test", "dummy");
		EXPECT(iniEdgeCase1.Generate(filename_INI_edge_case_1) == true);
		EXPECT(VerifyData(filename_INI_edge_case_1, data_INI_edge_case_1));

		// edge case 2: many empty sections
		mINI::INIFile iniEdgeCase2;
		iniEdgeCase2.Set("test1", "dummy", "");
		iniEdgeCase2.Remove("test1", "dummy");
		iniEdgeCase2.Set("test2", "dummy", "");
		iniEdgeCase2.Remove("test2", "dummy");
		iniEdgeCase2.Set("test3", "dummy", "");
		iniEdgeCase2.Remove("test3", "dummy");
		EXPECT(iniEdgeCase2.Generate(filename_INI_edge_case_2) == true);
		EXPECT(VerifyData(filename_INI_edge_case_2, data_INI_edge_case_2));

		// edge case 3: many empty sections with non-empty sections in between
		mINI::INIFile iniEdgeCase3;
		iniEdgeCase3.Set("test1", "dummy", "");
		iniEdgeCase3.Remove("test1", "dummy");
		iniEdgeCase3.Set("test2", "dummy", "");
		iniEdgeCase3.Remove("test2", "dummy");
		iniEdgeCase3.Set("realdata", {
			{"value1", "A"},
			{"value2", "B"},
			{"value3", "C"}
		});
		iniEdgeCase3.Set("test3", "dummy", "");
		iniEdgeCase3.Remove("test3", "dummy");
		EXPECT(iniEdgeCase3.Generate(filename_INI_edge_case_3) == true);
		EXPECT(VerifyData(filename_INI_edge_case_3, data_INI_edge_case_3));
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
