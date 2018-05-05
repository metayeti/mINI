///////////////////////////////////////////////////////////////////////////////
//
//  mINI
//  An INI file reader and writer.
//
//  (c) 2018 Danijel Durakovic
//  Licensed under terms of the MIT License
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

#include <string>
#include <algorithm>
#include <fstream>
#include <sys/stat.h>
#include "ini.h"


namespace mINI
{
	///////////////////////////////////////////////////////////////////////////
	//
	//  INIFile
	//
	///////////////////////////////////////////////////////////////////////////

	INIFile::INIFile()
	{
	}

	INIFile::INIFile(std::string const& filename)
	{
		Read(filename);
	}

	INIFile::~INIFile()
	{
	}

	bool INIFile::Has(std::string section) const
	{
		INIStringUtil::Trim(section);
		if (section.empty())
		{
			return false;
		}
		INIStringUtil::ToLower(section);
		for (auto const& it : data)
		{
			auto const& dSection = it.first;
			if (dSection == section)
			{
				return true;
			}
		}
		return false;
	}

	bool INIFile::Has(std::string section, std::string key) const
	{
		INIStringUtil::Trim(section);
		if (section.empty())
		{
			return false;
		}
		INIStringUtil::Trim(key);
		if (key.empty())
		{
			return false;
		}
		INIStringUtil::ToLower(section);
		INIStringUtil::ToLower(key);
		for (auto const& it : data)
		{
			auto const& dSection = it.first;
			if (dSection == section)
			{
				auto const& collection = it.second;
				for (auto const& it2 : collection)
				{
					auto const& dKey = it2.first;
					if (dKey == key)
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	size_t INIFile::Size() const
	{
		return data.size();
	}

	size_t INIFile::Size(std::string section) const
	{
		INIStringUtil::Trim(section);
		if (section.empty())
		{
			return false;
		}
		INIStringUtil::ToLower(section);
		for (auto const& it : data)
		{
			auto const& dSection = it.first;
			if (dSection == section)
			{
				auto const& collection = it.second;
				return collection.size();
			}
		}
		return 0;
	}

	INICollection* INIFile::Get(std::string section)
	{
		INIStringUtil::Trim(section);
		if (section.empty())
		{
			return nullptr;
		}
		INIStringUtil::ToLower(section);
		for (auto& it : data)
		{
			auto const& dSection = it.first;
			if (dSection == section)
			{
				return &it.second;
			}
		}
		return nullptr;
	}
	
	std::string INIFile::Get(std::string section, std::string key) const
	{
		INIStringUtil::Trim(section);
		if (section.empty())
		{
			return {};
		}
		INIStringUtil::Trim(key);
		if (key.empty())
		{
			return {};
		}
		INIStringUtil::ToLower(section);
		INIStringUtil::ToLower(key);
		for (auto const& it : data)
		{
			auto const& dSection = it.first;
			if (dSection == section)
			{
				auto const& collection = it.second;
				for (auto const& it2 : collection)
				{
					auto const& dKey = it2.first;
					if (dKey == key)
					{
						return it2.second;
					}
				}
			}
		}
		return {};
	}

	bool INIFile::GetBool(std::string section, std::string key) const
	{
		auto value = Get(section, key);
		INIStringUtil::ToLower(value);
		return (value != "" &&
				value != "no" &&
				value != "false" &&
				value != "0");
	}

	int INIFile::GetInt(std::string section, std::string key) const
	{
		auto const value = Get(section, key);
		return atoi(value.c_str());
	}

	char INIFile::GetChar(std::string section, std::string key) const
	{
		auto const value = Get(section, key);
		if (value.size())
		{
			return value.at(0);
		}
		return 0;
	}

	unsigned int INIFile::GetUInt(std::string section, std::string key) const
	{
		auto const value = Get(section, key);
		return strtoul(value.c_str(), NULL, 10);
	}

	double INIFile::GetDouble(std::string section, std::string key) const
	{
		auto const value = Get(section, key);
		return atof(value.c_str());
	}

	void INIFile::Set(std::string section, INICollection const& collection)
	{
		for (auto const& it : collection)
		{
			auto const& key = it.first;
			auto const& value = it.second;
			Set(section, key, value);
		}
	}
	
	void INIFile::Set(std::string section, std::string key, std::string value)
	{
		INIStringUtil::Trim(section);
		if (section.empty())
		{
			return;
		}
		INIStringUtil::Trim(key);
		if (key.empty())
		{
			return;
		}
		INIStringUtil::Trim(value);
		INIStringUtil::ToLower(section);
		INIStringUtil::ToLower(key);
		for (auto& it : data)
		{
			auto const& dSection = it.first;
			if (dSection == section)
			{
				auto& collection = it.second;
				for (auto& it2 : collection)
				{
					auto const& dKey = it2.first;
					if (dKey == key)
					{
						// update value
						it2.second = value;
						return;
					}
				}
				// add new key
				collection.push_back(INIKeyValue(key, value));
				return;
			}
		}
		// add new section and corresponding key/value
		INICollection collectionToAdd;
		collectionToAdd.push_back(INIKeyValue(key, value));
		data.push_back(INISection(section, collectionToAdd));
	}

	void INIFile::Set(std::string section, std::string key, const char* const value)
	{
		Set(section, key, std::string(value));
	}

	void INIFile::Set(std::string section, std::string key, bool value)
	{
		Set(section, key, std::string((value) ? "yes" : "no"));
	}

	void INIFile::Set(std::string section, std::string key, char value)
	{
		Set(section, key, std::string(1, value));
	}

	void INIFile::Set(std::string section, std::string key, int value)
	{
		Set(section, key, INIStringUtil::ToString(value));
	}

	void INIFile::Set(std::string section, std::string key, unsigned int value)
	{
		Set(section, key, INIStringUtil::ToString(value));
	}

	void INIFile::Set(std::string section, std::string key, double value)
	{
		Set(section, key, INIStringUtil::ToString(value));
	}

	bool INIFile::Remove(std::string section)
	{
		INIStringUtil::Trim(section);
		if (section.empty())
		{
			return false;
		}
		INIStringUtil::ToLower(section);
		auto it = data.begin();
		while (it != data.end())
		{
			auto const& dSection = it->first;
			if (dSection == section)
			{
				data.erase(it);
				return true;
			}
			++it;
		}
		return false;
	}

	bool INIFile::Remove(std::string section, std::string key)
	{
		INIStringUtil::Trim(section);
		if (section.empty())
		{
			return false;
		}
		INIStringUtil::Trim(key);
		if (key.empty())
		{
			return false;
		}
		INIStringUtil::ToLower(section);
		INIStringUtil::ToLower(key);
		for (auto& it : data)
		{
			auto const& dSection = it.first;
			if (dSection == section)
			{
				auto& collection = it.second;
				auto it2 = collection.begin();
				while (it2 != collection.end())
				{
					auto const& dKey = it2->first;
					if (dKey == key)
					{
						collection.erase(it2);
						return true;
					}
					++it2;
				}
			}
		}
		return false;
	}

	void INIFile::Clear()
	{
		data.clear();
	}

	bool INIFile::Read(std::string const& filename)
	{
		INIReadStream inputStream(filename);
		if (inputStream.Good())
		{
			data.clear();
			inputStream >> data;
			this->filename = filename;
			return true;
		}
		return false;
	}

	bool INIFile::Write(bool pretty) const
	{
		return Write(filename, pretty);
	}

	bool INIFile::Write(const char* const filename, bool pretty) const
	{
		return Write(std::string(filename), pretty);
	}

	inline bool INIFile::Write(std::string const& filename, bool pretty) const
	{
		if (filename.empty())
		{
			return false;
		}
		INILazyWriter lazyWriter(filename);
		lazyWriter.prettyPrint = pretty;
		return lazyWriter.Write(data);
	}

	bool INIFile::Generate(bool pretty) const
	{
		return Generate(filename, pretty);
	}
	
	bool INIFile::Generate(const char* const filename, bool pretty) const
	{
		return Generate(std::string(filename), pretty);
	}

	inline bool INIFile::Generate(std::string const& filename, bool pretty) const
	{
		if (filename.empty())
		{
			return false;
		}
		INIWriteStream outputStream(filename);
		if (outputStream.Good())
		{
		    outputStream.prettyPrint = pretty;
		    outputStream << data;
		    return true;
		}
		return false;
	}

	INIFile::iterator INIFile::begin()
	{
		return data.begin();
	}

	INIFile::iterator INIFile::end()
	{
		return data.end();
	}

	///////////////////////////////////////////////////////////////////////////
	//
	//  INIStringUtil
	//
	///////////////////////////////////////////////////////////////////////////

	const std::string INIStringUtil::whitespaceDelimiters = " \t\n\r\f\v";

	inline void INIStringUtil::Trim(std::string& str)
	{
		str.erase(str.find_last_not_of(whitespaceDelimiters) + 1);
		str.erase(0, str.find_first_not_of(whitespaceDelimiters));
	}

	inline void INIStringUtil::ToLower(std::string& str)
	{
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	}

	///////////////////////////////////////////////////////////////////////////
	//
	//  INIParser
	//
	///////////////////////////////////////////////////////////////////////////

	INIParser::PDataType INIParser::parseLine(std::string line, INIKeyValue& parseData)
	{
		parseData.first.clear();
		parseData.second.clear();
		INIStringUtil::Trim(line);
		if (line.empty())
		{
			return PDATA_NONE;
		}
		// get first character
		char firstCharacter = line[0];
		// check if comment
		if (firstCharacter == ';')
		{
			auto commentText = line.substr(1);
			INIStringUtil::Trim(commentText);
			// set data and return
			parseData.first = commentText;
			return PDATA_COMMENT;
		}
		// check if section
		if (firstCharacter == '[')
		{
			auto closingBracket = line.find_first_of(']');
			if (closingBracket != std::string::npos)
			{
				auto sectionName = line.substr(1, closingBracket - 1);
				INIStringUtil::Trim(sectionName);
				// make sure section is not an empty string
				if (!sectionName.empty())
				{
					// tolower section entry
					INIStringUtil::ToLower(sectionName);
					// set data and return
					parseData.first = sectionName;
					return PDATA_SECTION;
				}
			}
		}
		// check if key/value
		auto equalsAt = line.find_first_of('=');
		if (equalsAt != std::string::npos)
		{
			auto key = line.substr(0, equalsAt);
			INIStringUtil::Trim(key);
			// make sure key is not an empty string
			if (key.length() > 0)
			{
				INIStringUtil::ToLower(key);
				auto value = line.substr(equalsAt + 1);
				INIStringUtil::Trim(value);
				parseData.first = key;
				parseData.second = value;
			}
			return PDATA_KEYVALUE;
		}
		// unknown data
		return PDATA_UNKNOWN;
	}

	///////////////////////////////////////////////////////////////////////////
	//
	//  INIReadStream
	//
	///////////////////////////////////////////////////////////////////////////

	INIReadStream::INIReadStream(std::string const& filename, bool generateLineData)
	{
		fileReadStream.open(filename);
		if (generateLineData)
		{
			lineData = std::make_shared<T_LineData>();
		}
	}

	INIReadStream::~INIReadStream()
	{
	}

	bool INIReadStream::Good() const
	{
		return fileReadStream.is_open();
	}

	bool INIReadStream::operator>>(INISection& data)
	{
		bool eof = fileReadStream.eof();
		if (eof && sectionCurrent.empty())
		{
			return false;
		}
		std::string line;
		bool sectionChanged = false;
		INIKeyValue parseData;

		data.first = sectionCurrent;
		data.second.clear();

		while (!eof && std::getline(fileReadStream, line))
		{
			if (fileReadStream.bad())
			{
				return false;
			}
			if (lineData)
			{
				lineData->push_back(line);
			}
			auto parseResult = INIParser::parseLine(line, parseData);
			sectionChanged = false;
			if (parseResult == INIParser::PDATA_SECTION)
			{
				sectionChanged = true;
				sectionCurrent = parseData.first;
			}
			if (reading)
			{
				if (sectionChanged)
				{
					// finished parsing section
					return true;
				}
				if (parseResult == INIParser::PDATA_KEYVALUE)
				{
					auto const& key = parseData.first;
					auto const& value = parseData.second;
					data.second.push_back(INIKeyValue(key, value));
				}
			}
			else if (sectionChanged)
			{
				data.first = sectionCurrent;
				reading = true;
			}
		}

		sectionCurrent.clear();
		return reading;
	}

	size_t INIReadStream::operator>>(INIData& data)
	{
		INISection readData;
		size_t count = 0;
		while (*this >> readData)
		{
			data.push_back(readData);
			++count;
		}
		return count;
	}

	INIReadStream::T_LineDataPtr INIReadStream::GetLines()
	{
		return lineData;
	}

	///////////////////////////////////////////////////////////////////////////
	//
	//  INIWriteStream
	//
	///////////////////////////////////////////////////////////////////////////

	INIWriteStream::INIWriteStream(std::string const& filename)
	{
		fileWriteStream.open(filename);
	}

	INIWriteStream::~INIWriteStream()
	{
	}

	bool INIWriteStream::Good() const
	{
		return fileWriteStream.is_open();
	}

	void INIWriteStream::operator<<(INISection const& data)
	{
		std::string const& sectionName = data.first;
		auto const& collection = data.second;
		fileWriteStream << '[' << sectionName << ']';
		if (collection.size())
		{
			fileWriteStream << std::endl;
			auto it = collection.begin();
			for (;;)
			{
				std::string const& key = it->first;
				std::string const& value = it->second;
				fileWriteStream << key << ((prettyPrint) ? " = " : "=") << value;
				if (++it == collection.end())
				{
					break;
				}
				fileWriteStream << std::endl;
			}
		}
	}

	void INIWriteStream::operator<<(INIData const& data)
	{
		if (data.size())
		{
			auto it = data.begin();
			for (;;)
			{
				*this << *it;
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
		}
	}

	///////////////////////////////////////////////////////////////////////////
	//
	//  INILazyWriter
	//
	///////////////////////////////////////////////////////////////////////////

	INILazyWriter::T_DataMap INILazyWriter::MakeMap(INIData const& data)
	{
		T_DataMap dataMap;
		for (auto const& it : data)
		{
			auto const& section = it.first;
			auto const& collection = it.second;
			if (collection.empty())
			{
				continue;
			}
			T_KeyValueMap keyValueMap;
			for (auto const& it2 : collection)
			{
				auto const& key = it2.first;
				auto const& value = it2.second;
				keyValueMap[key] = value;
			}
			dataMap[section] = keyValueMap;
		}
		return dataMap;
	}

	inline INILazyWriter::T_LineData INILazyWriter::GetLazyOutput(INIData const& data) const
	{
		// read original data
		INIData originalData;
		T_LineData outputLines;
		INIReadStream::T_LineDataPtr originalLines;
		{
			INIReadStream inputStream(filename, true);
			if (inputStream.Good())
			{
				inputStream >> originalData;
				originalLines = inputStream.GetLines();
			}
			else
			{
				return outputLines;
			}
		}
		// build data maps
		T_DataMap originalMap = MakeMap(originalData);
		T_DataMap outputMap = MakeMap(data);

		// iterate over original lines and write to output
		INIKeyValue parseData;
		std::string sectionCurrent;
		bool parsingSection = false;
		bool continueToNextSection = false;
		bool discardNextEmpty = false;
		bool writeNewKeys = false;
		size_t lastKeyLine = 0;

		for (auto line = originalLines->begin(); line != originalLines->end(); ++line)
		{
			if (!writeNewKeys)
			{
				auto parseResult = INIParser::parseLine(*line, parseData);
				if (parseResult == INIParser::PDATA_SECTION)
				{
					if (parsingSection)
					{
						// encountered a new section while already parsing a section
						// backtrack and write any remaining keys
						--line;
						writeNewKeys = true;
						parsingSection = false;
						continue;
					}
					sectionCurrent = parseData.first;

					// check if section exists in output data
					if (!outputMap.count(sectionCurrent))
					{
						continueToNextSection = true;
						discardNextEmpty = true;
					}
					else
					{
						// new section encountered
						parsingSection = true;
						continueToNextSection = false;
						discardNextEmpty = false;
						outputLines.push_back(*line);
					}
					continue;
				}
				else if (parseResult == INIParser::PDATA_KEYVALUE)
				{
					if (continueToNextSection)
					{
						continue;
					}
					auto& keyValueMap = outputMap[sectionCurrent];
					auto const& key = parseData.first;
					auto const& value = parseData.second;
					// check if key exists in output data
					if (keyValueMap.count(key))
					{
						auto& outputValue = keyValueMap[key];
						// check if value changed
						if (value == outputValue)
						{
							// no changes, add line to output
							outputLines.push_back(*line);
						}
						else
						{
							// change was made to this key
							// determine value offset
							auto equalsAt = line->find_first_of('=');
							auto valueAt = line->find_first_not_of(
								INIStringUtil::whitespaceDelimiters,
								equalsAt + 1
							);
							// construct a line with new value
							std::string keyValueLine = line->substr(0, valueAt);
							if (prettyPrint && equalsAt + 1 == valueAt)
							{
								keyValueLine += ' ';
							}
							keyValueLine += outputValue;
							outputLines.push_back(keyValueLine);
						}
						// keep a a reference to current line
						lastKeyLine = outputLines.size();
					}
				}
				else
				{
					if (discardNextEmpty && line->empty())
					{
						// skip a blank line if we just discarded a section
						discardNextEmpty = false;
					}
					else
					{
						// unknown data, write back to output
						outputLines.push_back(*line);
					}
				}
			}
			if (writeNewKeys || std::next(line) == originalLines->end())
			{
				// check if any keys need to be added before moving on to next section
				auto keyValueMap = originalMap[sectionCurrent];
				T_LineData linesToAdd;
				// we want to add items in order of insertion, use input data
				for (auto const& it : data)
				{
					auto const& dSection = it.first;
					if (dSection == sectionCurrent)
					{
						auto const& collection = it.second;
						for (auto const& it2 : collection)
						{
							auto const& key = it2.first;
							if (keyValueMap.count(key))
							{
								continue;
							}
							// add a new key/value
							auto const& value = it2.second;
							linesToAdd.push_back(
								key + ((prettyPrint) ? " = " : "=") + value
							);
						}
					}
				}
				if (!linesToAdd.empty())
				{
					outputLines.insert(
						outputLines.begin() + lastKeyLine,
						linesToAdd.begin(),
						linesToAdd.end()
					);
				}
				if (writeNewKeys)
				{
					// backtrack so we can keep parsing
					--line;
					writeNewKeys = false;
				}
			}
		}

		// check for any new sections and add them to the end of the file
		for (auto const& it : outputMap)
		{
			auto const& section = it.first;
			if (originalMap.count(section))
			{
				continue;
			}
			// add section and corresponding key/value pairs
			if (prettyPrint && outputLines.size() > 0 && !outputLines.back().empty())
			{
				outputLines.push_back({});
			}
			outputLines.push_back('[' + section + ']');
			// we want to insert keys in-order, use input data
			for (auto const& it2 : data)
			{
				auto const& dSection = it2.first;
				if (dSection == section)
				{
					auto const& collection = it2.second;
					for (auto const& it3 : collection)
					{
						auto const& key = it3.first;
						auto const& value = it3.second;
						outputLines.push_back(
							key + ((prettyPrint) ? " = " : "=") + value
						);
					}
				}
			}
		}

		return outputLines;
	}

	INILazyWriter::INILazyWriter(std::string const& filename)
	{
		this->filename = filename;
	}

	INILazyWriter::~INILazyWriter()
	{
	}

	bool INILazyWriter::Write(INIData const& data) const
	{
		struct stat buf;
		bool fileExists = (stat(filename.c_str(), &buf) == 0);
		if (!fileExists)
		{
			// file doesn't exist, delegate writing to INIWriteStream
			INIWriteStream writeStream(filename);
			if (writeStream.Good())
			{
				writeStream.prettyPrint = prettyPrint;
				writeStream << data;
				return true;
			}
			return false;
		}
		// file exists, lazy-write to it
		std::ofstream fileWriteStream(filename);
		if (fileWriteStream.is_open())
		{
			T_LineData output = GetLazyOutput(data);
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
		return false;
	}
}
