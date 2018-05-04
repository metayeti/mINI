#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include "lest.hpp"
#include "mini/ini.h"

typedef std::vector<std::string> T_LineData;

//
// test data
//
const T_LineData INI_file_well_formed = {
	";string values",
	"[fruit]",
	"apple=good",
	"banana=very good",
	"grape=supreme",
	"orange=fantastic",
	";number values",
	"[vegetables]",
	"garlic=-3",
	"pepper=0.76",
	"pumpkin=-2",
	";booleans",
	"[nuts]",
	"almond=false",
	"walnut=0",
	"peanut=",
	"cashew=no",
	"coconut=yes"
};

const T_LineData INI_file_not_well_formed = {
	"GARBAGE",
	"",
	"; ;; ; ;;;",
	"      ;  string values    ",
	"",
	"[fruit]          ",
	"",
	"",
	"   GARBAGE",
	"apple= good",
	"GARB    AGE    ",
	"banana =           very good",
	"   GARBAGE  ",
	"grape=supreme",
	"",
	"GARBAGE",
	"",
	"orange =fantastic",
	"",
	"",
	"GARBAGE",
	"GARBAGE",
	"",
	"GARBAGE",
	";number values",
	"GARBAGE",
	"[    vegetables    ]  ",
	"GARBAGE",
	"GARBAGE",
	"GARBAGE",
	"GARBAGE",
	"",
	"garlic = -3",
	"GARBAGE",
	"pepper= 0.76",
	"GARBAGE",
	"GARBAGE",
	"pumpkin =-2",
	";booleans",
	"",
	"GARBAGE",
	"               [ nuts ]               ",
	"",
	"",
	"",
	"",
	"",
	"",
	"almond=false",
	"GARBAGE",
	"GARBAGE",
	"  walnut=                  0",
	"  peanut=                    ",
	"cashew                  =no",
	"GARBAGE",
	"",
	"     coconut=   yes"
	"",
	"GARBAGE",
	"GARBAGE",
	"",
	"GARBAGE",
	"GARBAGE",
	"GARBAGE",
};

const std::string filename1 = "data1.ini";
const std::string filename2 = "data2.ini";

bool WriteTestINI(std::string const& filename, T_LineData const& lines)
{
	std::ofstream fileWriteStream(filename);
	if (fileWriteStream.is_open())
	{
		for (auto it = lines.begin(); it != lines.end(); ++it)
		{
			fileWriteStream << *it;
			if (it != lines.end())
			{
				fileWriteStream << std::endl;
			}
		}
		return true;
	}
	return false;
}

void OutputData(std::string const& filename, mINI::INIFile& ini)
{
	std::cout << '=' << filename << '=' << std::endl;
	for (auto it = ini.begin(); it != ini.end(); ++it)
	{
		std::cout << it->first << std::endl;
		auto collection = it->second;
		for (auto it2 = collection.begin(); it2 != collection.end(); ++it2)
		{
			auto const& key = it2->first;
			auto const& value = it2->second;
			std::cout << "   " << key << ": " << value << std::endl;
		}
	}
	std::cout << std::endl;
}

//
// test cases
//
const lest::test mINI_tests[] =
{
	// read two INI files and compare results
	CASE("Read INI: General")
	{
		mINI::INIFile iniDataA(filename1);
		mINI::INIFile iniDataB(filename2);
		// cout all data
		OutputData(filename1, iniDataA);
		OutputData(filename2, iniDataB);
		// check data
		EXPECT(iniDataA.Get("fruit", "apple") == iniDataB.Get("fruit", "apple"));
		EXPECT(iniDataA.Get("fruit", "banana") == iniDataB.Get("fruit", "banana"));
		EXPECT(iniDataA.Get("fruit", "grape") == iniDataB.Get("fruit", "grape"));
		EXPECT(iniDataA.Get("fruit", "orange") == iniDataB.Get("fruit", "orange"));
		EXPECT(iniDataA.GetUInt("vegetables", "garlic") == iniDataB.GetUInt("vegetables", "garlic"));
		EXPECT(fabs(iniDataA.GetFloat("vegetables", "pepper") - iniDataB.GetFloat("vegetables", "pepper")) < 0.1);
		EXPECT(iniDataA.GetInt("vegetables", "pumpkin") == iniDataB.GetInt("vegetables", "pumpkin"));
		EXPECT(iniDataA.GetBool("nuts", "almond") == iniDataB.GetBool("nuts", "almond"));
		EXPECT(iniDataA.GetBool("nuts", "walnut") == iniDataB.GetBool("nuts", "walnut"));
		EXPECT(iniDataA.GetBool("nuts", "peanut") == iniDataB.GetBool("nuts", "peanut"));
		EXPECT(iniDataA.GetBool("nuts", "cashew") == iniDataB.GetBool("nuts", "cashew"));
		EXPECT(iniDataA.GetBool("nuts", "coconut") == iniDataB.GetBool("nuts", "coconut"));
	}
};

int main(int argc, char** argv)
{
	// write test files
	WriteTestINI(filename1, INI_file_well_formed);
	WriteTestINI(filename2, INI_file_not_well_formed);
	// run tests
	if (int failures = lest::run(mINI_tests, argc, argv))
	{
		return failures;
	}

	return std::cout << "All tests passed!" << std::endl, EXIT_SUCCESS;
}
