///////////////////////////////////////////////////////////////////////////////
//
//  mINI
//  An INI file reader and writer.
//
//  (c) 2018 Danijel Durakovic
//  Licensed under terms of the MIT License
//
///////////////////////////////////////////////////////////////////////////////
//
//  A tiny-ish utility for reading from and writing data to INI files with a
//  straightforward API and a minimal footprint. It conforms to the (somewhat)
//  standard INI format - sections and keys are case insensitive, and all
//  leading and trailing whitespace is ignored. Comments are lines that begin
//  with a semicolon. Trailing comments are not allowed since values may also
//  contain semicolons.
//
//  Files are read on demand in one fell swoop and the data is kept in memory,
//  ready to be manipulated. Files are closed after read or write operations.
//
//  This utility supports lazy writing, which only writes changes and updates
//  and preserves custom spacings and comments. A lazy write will read the
//  output file, find changes made and update the file accordingly. If
//  performance is a strong issue and you only need to generate files, use
//  Generate instead.
//
//  Section and key order is preserved on both read and write operations.
//
//  Instantiate INIFile and use:
//  - the constructor or Read to read from an INI file.
//  - Has to check if a given section or key exists.
//  - Size to get number of sections of keys within a section.
//  - Get, GetInt, GetUInt... to retreive data.
//  - Set to set a new value.
//  - RemoveKey or RemoveSection to remove parts of data.
//  - Clear to remove all data.
//  - Write to write to and update an INI file.
//  - Generate to create or rewrite an INI file.
//
//  You can use the iterators at begin() and end() to iterate through all data
//  in-order.
//
//  Long live the INI file!!!
//
///////////////////////////////////////////////////////////////////////////////

/*
 * The MIT License (MIT)
 * Copyright (c) 2018 Danijel Durakovic
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef MINI_INI_H_
#define MINI_INI_H_

#include <string>
#include <sstream>
#include <vector>
#include <fstream>
#include <utility>
#include <memory>
#include <map>

namespace mINI
{
	typedef std::pair<std::string, std::string> INIKeyValue;
	typedef std::vector<INIKeyValue> INICollection;
	typedef std::pair<std::string, INICollection> INISection;
	typedef std::vector<INISection> INIData;

	class INIFile
	{
	private:
		INIData data;
		std::string filename;

	public:
		typedef INIData::iterator iterator;
		typedef INIData::const_iterator const_iterator;

		// construct a blank file
		INIFile();
		// read data from file on construction
		INIFile(std::string const& filename);

		~INIFile();

		// check if a given section exists
		bool Has(std::string section) const;

		// check if a given key exists
		bool Has(std::string section, std::string key) const;

		// get number of sections
		size_t Size() const;

		// get number of keys in a given section
		size_t Size(std::string section) const;

		// get a string value
		std::string Get(std::string section, std::string key) const;

		// get a boolean value
		// "", "no", "false" and "0" (case-insensitive) map to false
		// all other values map to true
		bool GetBool(std::string section, std::string key) const;

		// get a char value
		char GetChar(std::string section, std::string key) const;

		// get an integer value
		int GetInt(std::string section, std::string key) const;

		// get an unsigned integer value
		unsigned int GetUInt(std::string section, std::string key) const;

		// get a float value
		float GetFloat(std::string section, std::string key) const;

		// get a double value
		double GetDouble(std::string section, std::string key) const;

		// set a string value
		void Set(std::string section, std::string key, std::string value);
		void Set(std::string section, std::string key, const char* const value);

		// set a boolean value
		void Set(std::string section, std::string key, bool value);

		// set a char value
		void Set(std::string section, std::string key, char value);

		// set an integer value
		void Set(std::string section, std::string key, int value);

		// set an unsigned integer value
		void Set(std::string section, std::string key, unsigned int value);

		// set a float value
		void Set(std::string section, std::string key, float value);

		// set a double value
		void Set(std::string section, std::string key, double value);

		// remove a section
		bool Remove(std::string section);

		// remove a key
		bool Remove(std::string section, std::string key);

		// clear all data
		void Clear();

		// read data from a file
		bool Read(std::string const& filename);

		// lazy-write to file
		// preserves comments, spacings, etc. and updates file with changes
		bool Write(bool pretty = false) const;
		bool Write(const char* const filename, bool pretty = false) const;
		bool Write(std::string const& filename, bool pretty = false) const;

		// generate a file
		// this will overwrite any previous file contents
		bool Generate(bool pretty = false) const;
		bool Generate(const char* const filename, bool pretty = false) const;
		bool Generate(std::string const& filename, bool pretty = false) const;

		// iterators
		iterator begin();
		iterator end();
	};

	class INIStringUtil
	{
	public:
		static const std::string whitespaceDelimiters;
		static inline void Trim(std::string& str);
		static inline void ToLower(std::string& str);

		template<typename T>
		static std::string ToString(T const& value)
		{
			std::stringstream buff;
			std::string output;
			buff.precision(12);
			buff << value;
			buff >> output;
			return output;
		}
	};

	class INIParser
	{
	public:
		enum PDataType
		{
			PDATA_NONE,
			PDATA_COMMENT,
			PDATA_SECTION,
			PDATA_KEYVALUE,
			PDATA_UNKNOWN
		};
		
		static PDataType parseLine(std::string line, INIKeyValue& parseData);
	};

	class INIReadStream
	{
	public:
		typedef std::vector<std::string> T_LineData;
		typedef std::shared_ptr<T_LineData> T_LineDataPtr;

	private:
		std::ifstream fileReadStream;
		std::string sectionCurrent;
		bool reading = false;
		T_LineDataPtr lineData;

	public:
		INIReadStream(std::string const& filename, bool generateLineData = false);
		~INIReadStream();

		// checks if opening succeeded
		bool Good() const;

		// read next section
		// returns true if there is more to parse or false on eof or error
		bool operator>>(INISection& data);

		// read all data at once
		// returns number of sections
		size_t operator>>(INIData& data);

		T_LineDataPtr GetLines();
	};

	class INIWriteStream
	{
	private:
		std::ofstream fileWriteStream;

	public:
		bool prettyPrint = false;

		INIWriteStream(std::string const& filename);
		~INIWriteStream();

		// checks if opening succeeded
		bool Good() const;

		// writes a single section
		void operator<<(INISection const& data);

		// writes all data at once
		void operator<<(INIData const& data);
	};

	class INILazyWriter
	{
	private:
		typedef std::vector<std::string> T_LineData;
		typedef std::map<std::string, std::string> T_KeyValueMap;
		typedef std::map<std::string, T_KeyValueMap> T_DataMap;

		std::string filename;

		static T_DataMap MakeMap(INIData const& data);
		T_LineData GetLazyOutput(INIData const& data) const;

	public:
		bool prettyPrint = false;

		INILazyWriter(std::string const& filename);
		~INILazyWriter();

		// write changes to file
		bool Write(INIData const& data) const;
	};
}

#endif // MINI_INI_H_
