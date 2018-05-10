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

///////////////////////////////////////////////////////////////////////////////
//
//  /mINI/
//  An INI file reader and writer for the modern age.
//
//  (c) 2018 Danijel Durakovic
//  Licensed under terms of the MIT License
//
///////////////////////////////////////////////////////////////////////////////
//
//  A tiny-ish utility for reading from and writing data to INI files with a
//  straightforward API and a minimal footprint. It conforms to the (somewhat)
//  standard INI format - sections and keys are case insensitive, and all
//  leading and trailing whitespace is ignored. Empty keys and sections names
//  are ignored. Comments are lines that begin with a semicolon. Trailing
//  comments on value lines are not allowed since values may also contain
//  semicolons. Trailing comments on section lines are ignored.
//
//  Files are read on demand in one fell swoop and the data is kept in memory,
//  ready to be manipulated. Files are closed after read or write operations.
//  This utility supports lazy writing, which only writes changes and updates
//  and preserves custom spacings and comments. A lazy write invoked by a
//  write() call will read the output file, find changes made and update the
//  file accordingly. If performance is a strong issue and you only need to
//  generate files, use generate() instead. Section and key order is preserved
//  on both read and write operations.
//
///////////////////////////////////////////////////////////////////////////////
//
//  /* === BASIC READ/WRITE === */
//
//  /* read from file */
//  mINI::INIFile file("myfile.ini");
//  mINI::INIStructure ini;
//  file.read(ini);
//
//  /* read values. if key doesn't exist it will be created */
//  std::string value = ini["section"]["key"];
//
//  /* read values safely. if key doesn't exist it will NOT be created */
//  std::string value = ini.get("section").get("key");
//
//  /* =IMPORTANT=
//     The difference between the [] and get() operations is that [] returns
//     REAL data which you can modify and creates a new item automatically
//     if it doesn't yet exist, while get() returns a COPY of data and
//     doesn't create new keys. Use has() combined with the [] operator to
//     get full control over what is being created anew in the structure. */
//
//  /* set or update values */
//  ini["section"]["key"] = "value";
//
//  /* set multiple values */
//  ini["section2"].set({
//      {"key1", "value1"},
//      {"key2", "value2"}
//  });
//
//  /* write updates back to file, preserving comments and formatting */
//  file.write(ini);
//
//  /* or generate a file */
//  file.generate(ini);
//
//  /* === OTHER FEATURES === */
//
//  /* check if section or key is present */
//  bool hasSection = ini.has("section");
//  bool hasKey = ini["section"].has("key");
//
//  /* remove keys or sections */
//  bool removedKeySuccess = ini["section2"].remove("key2");
//  bool removedSectionSuccess = ini.remove("section2");
//
//  /* check for number of keys or sections*/
//  size_t n_keys = ini["section"].size();
//  size_t n_sections = ini.size();
//
//  /* to remove all keys from a section */
//  ini["section"].clear();
//
//  /* to clear all data */
//  ini.clear();
//
//  /* =IMPORTANT=
//     Keep in mind that [] will always create an item if it does not already
//     exist! You can use has() to check if an item exists before performing
//     further operations. Remember that get() will return a copy of data, so
//     you should NOT do removes or updates that way. Straightforward usage
//     with [] operators shouldn't really be a problem in most real-world cases
//     where you're doing lookups on known keys anyway and you may not care if
//     empty keys or sections get created, but this is something to keep in mind
//     when dealing with this datastructure. Always use has() before using the
//     [] operator IF you don't want new empty sections and keys. Below is
//     a short example that demonstrates safe manipulation of data. */
//
//  if (ini.has("section"))
//  {
//      // we have section, we can access it safely
//      auto& collection = ini["section"];
//      if (collection.has("key"))
//      {
//          // we have key, we can access it safely
//          auto& value = collection["key"];
//          // do something with value, for example change it
//          value = "some value";
//          // if we wanted to, we can remove this key safely since we know
//          // that it exists
//          collection.remove("key");
//      }
//  }
//
//  /* to iterate through data in-order and display results */
//  for (auto const& it : ini)
//  {
//      auto const& section = it.first();
//      auto const& collection = *it.second();
//      std::cout << "[" << section << "]" << std::endl;
//      for (auto const& it2 : collection)
//      {
//           auto const& key = it2.first();
//           auto const& value = *it2.second();
//           std::cout << key << "=" << value << std::endl;
//      }
//  }
//
///////////////////////////////////////////////////////////////////////////////
//
//  Long live the INI file!!!
//
///////////////////////////////////////////////////////////////////////////////

#ifndef MINI_INI_H_
#define MINI_INI_H_

#include <string>
#include <sstream>
#include <algorithm>
#include <utility>
#include <unordered_map>
#include <vector>
#include <memory>
#include <fstream>
#include <sys/stat.h>

namespace mINI
{
	class INIStringUtil
	{
	public:
		static const std::string whitespaceDelimiters;
		static inline void Trim(std::string& str)
		{
			str.erase(str.find_last_not_of(whitespaceDelimiters) + 1);
			str.erase(0, str.find_first_not_of(whitespaceDelimiters));
		}
		static inline void ToLower(std::string& str)
		{
			std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		}
	};

	const std::string INIStringUtil::whitespaceDelimiters = " \t\n\r\f\v";

	template<class T>
	class INIMap
	{
	private:
		using T_Data = std::vector<std::unique_ptr<T>>;
		using T_DataIndexMap = std::unordered_map<std::string, std::size_t>;
		using T_IterItem = std::pair<std::string, const T*>;
		using T_IterList = std::vector<T_IterItem>;
		using T_MultiArgs = typename std::vector<std::pair<std::string, T>>;

		T_Data data;
		T_DataIndexMap dataIndexMap;
		T_IterList iterList;

		inline std::size_t setEmpty(std::string& key)
		{
			std::size_t index = data.size();
			dataIndexMap[key] = index;
			data.push_back(std::make_unique<T>());
			iterList.push_back(T_IterItem(key, data.back().get()));
			return index;
		}

	public:
		using const_iterator = typename T_IterList::const_iterator;

		INIMap() { }

		INIMap(INIMap const& other)
		{
			std::size_t data_size = other.data.size();
			for (std::size_t i = 0; i < data_size; ++i)
			{
				std::string const& key = other.iterList[i].first;
				auto const& ptrToData = other.data[i];
				data.push_back(std::make_unique<T>(*ptrToData));
				iterList.push_back(T_IterItem(key, data.back().get()));
			}
			dataIndexMap = T_DataIndexMap(other.dataIndexMap);
		}

		T& operator[](std::string key)
		{
			INIStringUtil::Trim(key);
			INIStringUtil::ToLower(key);
			auto it = dataIndexMap.find(key);
			bool hasIt = (it == dataIndexMap.end());
			std::size_t index = (hasIt) ? setEmpty(key) : it->second;
			return *data[index];
		}

		T get(std::string key) const
		{
			INIStringUtil::Trim(key);
			INIStringUtil::ToLower(key);
			auto it = dataIndexMap.find(key);
			if (it == dataIndexMap.end())
			{
				return T();
			}
			return T(*data[it->second]);
		}

		bool has(std::string key) const
		{
			INIStringUtil::Trim(key);
			INIStringUtil::ToLower(key);
			return (dataIndexMap.count(key) == 1);
		}

		void set(std::string key, T obj)
		{
			INIStringUtil::Trim(key);
			INIStringUtil::ToLower(key);
			auto it = dataIndexMap.find(key);
			if (it != dataIndexMap.end())
			{
				data[it->second] = std::make_unique<T>(obj);
			}
			else
			{
				dataIndexMap[key] = data.size();
				data.push_back(std::make_unique<T>(obj));
				iterList.push_back(T_IterItem(key, data.back().get()));
			}
		}

		void set(T_MultiArgs const& multiArgs)
		{
			for (auto const& it : multiArgs)
			{
				auto const& key = it.first;
				auto const& obj = it.second;
				set(key, obj);
			}
		}

		bool remove(std::string key)
		{
			INIStringUtil::Trim(key);
			INIStringUtil::ToLower(key);
			auto it = dataIndexMap.find(key);
			if (it != dataIndexMap.end())
			{
				auto index = it->second;
				data.erase(data.begin() + index);
				iterList.erase(iterList.begin() + index);
				dataIndexMap.erase(it);
				for (auto& it2 : dataIndexMap)
				{
					auto& vi = it2.second;
					if (vi > index)
					{
						vi--;
					}
				}
				return true;
			}
			return false;
		}

		void clear()
		{
			data.clear();
			iterList.clear();
			dataIndexMap.clear();
		}

		std::size_t size() const
		{
			return data.size();
		}

		const_iterator begin() const { return iterList.begin(); }
		const_iterator end() const { return iterList.end(); }
	};

	using INIStructure = INIMap<INIMap<std::string>>;

	class INIParser
	{
	public:
		using T_ParseValues = std::pair<std::string, std::string>;

		enum PDataType
		{
			PDATA_NONE,
			PDATA_COMMENT,
			PDATA_SECTION,
			PDATA_KEYVALUE,
			PDATA_UNKNOWN
		};

		static PDataType parseLine(std::string line, T_ParseValues& parseData)
		{
			parseData.first.clear();
			parseData.second.clear();
			INIStringUtil::Trim(line);
			if (line.empty())
			{
				return PDATA_NONE;
			}
			char firstCharacter = line[0];
			if (firstCharacter == ';')
			{
				return PDATA_COMMENT;
			}
			if (firstCharacter == '[')
			{
				auto closingBracketAt = line.find_first_of(']');
				if (closingBracketAt != std::string::npos)
				{
					auto section = line.substr(1, closingBracketAt - 1);
					INIStringUtil::Trim(section);
					if (!section.empty())
					{
						parseData.first = section;
						return PDATA_SECTION;
					}
				}
			}
			auto equalsAt = line.find_first_of('=');
			if (equalsAt != std::string::npos)
			{
				auto key = line.substr(0, equalsAt);
				INIStringUtil::Trim(key);
				if (key.length() > 0)
				{
					auto value = line.substr(equalsAt + 1);
					INIStringUtil::Trim(value);
					parseData.first = key;
					parseData.second = value;
					return PDATA_KEYVALUE;
				}
			}
			return PDATA_UNKNOWN;
		}
	};

	class INIReader
	{
	public:
		using T_LineData = std::vector<std::string>;
		using T_LineDataPtr = std::shared_ptr<T_LineData>;

	private:
		std::ifstream fileReadStream;
		T_LineDataPtr lineData;

	public:
		INIReader(std::string const& filename, bool keepLineData = false)
		{
			fileReadStream.open(filename);
			if (keepLineData)
			{
				lineData = std::make_shared<T_LineData>();
			}
		}
		~INIReader() { }

		bool operator>>(INIStructure& data)
		{
			if (!fileReadStream.is_open())
			{
				return false;
			}
			std::string line;
			std::string section;
			bool inSection = false;
			INIParser::T_ParseValues parseData;
			while (std::getline(fileReadStream, line))
			{
				auto parseResult = INIParser::parseLine(line, parseData);
				if (parseResult == INIParser::PDATA_SECTION)
				{
					inSection = true;
					section = parseData.first;
				}
				else if (inSection && parseResult == INIParser::PDATA_KEYVALUE)
				{
					auto const& key = parseData.first;
					auto const& value = parseData.second;
					data[section][key] = value;
				}
				if (lineData && parseResult != INIParser::PDATA_UNKNOWN)
				{
					lineData->push_back(line);
				}
			}
			return true;
		}

		T_LineDataPtr getLines()
		{
			return lineData;
		}
	};

	class INIGenerator
	{
	private:
		std::ofstream fileWriteStream;

	public:
		bool prettyPrint = false;

		INIGenerator(std::string const& filename)
		{
			fileWriteStream.open(filename);
		}
		~INIGenerator() { }

		bool operator<<(INIStructure const& data)
		{
			if (!fileWriteStream.is_open())
			{
				return false;
			}
			if (!data.size())
			{
				return true;
			}
			auto it = data.begin();
			for (;;)
			{
				auto const& section = it->first;
				auto const& collection = *it->second;
				fileWriteStream
					<< "["
					<< section
					<< "]";
				if (collection.size())
				{
					fileWriteStream << std::endl;
					auto it2 = collection.begin();
					for (;;)
					{
						auto const& key = it2->first;
						auto const& value = *it2->second;
						fileWriteStream
							<< key
							<< ((prettyPrint) ? " = " : "=")
							<< value;
						if (++it2 == collection.end())
						{
							break;
						}
						fileWriteStream << std::endl;
					}
				}
				if (++it == data.end())
				{
					break;
				}
				fileWriteStream << std::endl;
				if (prettyPrint)
				{
					fileWriteStream << std::endl;
				}
			}
			return true;
		}
	};

	class INIWriter
	{
	private:
		using T_LineData = std::vector<std::string>;
		std::string filename;

		T_LineData getLazyOutput(INIStructure const& data, INIStructure const& original)
		{
			if (prettyPrint)
			{
			}
		}

	public:
		bool prettyPrint = false;

		INIWriter(std::string const& filename)
		: filename(filename)
		{
		}
		~INIWriter() { }

		bool operator<<(INIStructure const& data)
		{
			struct stat buf;
			bool fileExists = (stat(filename.c_str(), &buf) == 0);
			if (!fileExists)
			{
				INIGenerator generator(filename);
				generator.prettyPrint = prettyPrint;
				return generator << data;
			}
			INIStructure originalData;
			INIReader::T_LineDataPtr lineData;
			bool readSuccess = false;
			{
				INIReader reader(filename, true);
				if (readSuccess = reader >> originalData)
				{
					lineData = reader.getLines();
				}
			}
			if (readSuccess)
			{
				T_LineData output = getLazyOutput(data, originalData);
				std::ofstream fileWriteStream(filename);
				if (fileWriteStream.is_open())
				{
					if (output.size())
					{
						auto line = output.begin();
						for (;;)
						{
							fileWriteStream << *line;
							if (++line == output.end())
							{
								break;
							}
							fileWriteStream << std::endl;
						}
					}
					return true;
				}
			}
			return false;
		}
	};

	class INIFile
	{
	private:
		std::string filename;

	public:
		INIFile(std::string const& filename)
		: filename(filename)
		{ }

		~INIFile() { }

		bool read(INIStructure& data) const
		{
			if (data.size())
			{
				data.clear();
			}
			if (filename.empty())
			{
				return false;
			}
			INIReader reader(filename);
			return reader >> data;
		}

		bool generate(INIStructure const& data, bool pretty = false) const
		{
			if (filename.empty())
			{
				return false;
			}
			INIGenerator generator(filename);
			generator.prettyPrint = pretty;
			return generator << data;
		}

		bool write(INIStructure const& data, bool pretty = false) const
		{
			if (filename.empty())
			{
				return false;
			}
			INIWriter writer(filename);
			writer.prettyPrint = pretty;
			return writer << data;
		}
	};
}

#endif // MINI_INI_H_
