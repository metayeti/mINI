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
//  with a semicolon. Trailing comments on value lines are not allowed since
//  values may also contain semicolons. Trailing comments on section lines are
//  ignored and therefore implicitly allowed.
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
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <memory>
#include <fstream>

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
	class MagicMap
	{
	public:
		using T_Data = std::vector<std::unique_ptr<T>>;
		using T_DataIndexMap = std::unordered_map<std::string, std::size_t>;
		using T_IterItem = std::pair<std::string, const T&>;
		using T_IterList = std::vector<T_IterItem>;
		using const_iterator = typename T_IterList::const_iterator;

	private:
		T_Data data;
		T_DataIndexMap dataIndexMap;
		T_IterList iterList;

		inline std::size_t SetEmpty(std::string& key)
		{
			std::size_t index = data.size();
			dataIndexMap[key] = index;
			data.push_back(std::make_unique<T>());
			iterList.push_back(T_IterItem(key, *data.back()));
			return index;
		}

	public:
		MagicMap() { }

		MagicMap(MagicMap const& other)
		{
			std::size_t data_size = other.data.size();
			for (std::size_t i = 0; i < data_size; ++i)
			{
				std::string const& key = other.iterList[i].first;
				auto const& ptrToData = other.data[i];
				data.push_back(std::make_unique<T>(*ptrToData));
				iterList.push_back(T_IterItem(key, *data.back()));
			}
			dataIndexMap = T_DataIndexMap(other.dataIndexMap);
		}

		T& operator[](std::string key)
		{
			INIStringUtil::Trim(key);
			INIStringUtil::ToLower(key);
			auto it = dataIndexMap.find(key);
			bool hasIt = (it == dataIndexMap.end());
			std::size_t index = (hasIt) ? SetEmpty(key) : it->second;
			return *data[index];
		}

		T get(std::string key)
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

		bool has(std::string key)
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
				iterList.push_back(T_IterItem(key, *data.back()));
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

		std::size_t size()
		{
			return data.size();
		}

		const_iterator begin() const { return iterList.begin(); }
		const_iterator end() const { return iterList.end(); }
	};

	using INISection = MagicMap<std::string>;
	using INIStructure = MagicMap<INISection>;
	using INIKeyValue = std::pair<std::string, std::string>;

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

		static PDataType parseLine(std::string line, INIKeyValue& parseData)
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
				auto commentText = line.substr(1);
				INIStringUtil::Trim(commentText);
				parseData.first = commentText;
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
						INIStringUtil::ToLower(section);
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
					INIStringUtil::ToLower(key);
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
	private:
		std::ifstream fileReadStream;

	public:
		INIReader(std::string const& filename)
		{
			fileReadStream.open(filename);
		}
		~INIReader() { }

		bool good()
		{
			return fileReadStream.is_open();
		}

		void operator>>(INIStructure& data)
		{
			std::string line;
			std::string section;
			bool inSection = false;
			INIKeyValue parseData;
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
			}
		}
	};

	class INIWriter
	{
	};

	class INILazyWriter
	{
	};

	class INIFile
	{
	private:
		std::string filename;

	public:
		INIFile(const std::string& filename)
		: filename(filename)
		{
		}

		~INIFile() { }

		INIStructure read()
		{
			INIStructure output;
			INIReader reader(filename);
			if (reader.good())
			{
				reader >> output;
			}
			return output;
		}

		bool Write(INIStructure data)
		{
			return false;
		}

		bool Generate(INIStructure data)
		{
			return false;
		}
	};
}

#endif // MINI_INI_H_
