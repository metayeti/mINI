# mINI

**This project is still being developed!**

This is a tiny utility library for reading from and writing data to INI files with a straightforward API and a minimal footprint. It conforms to the (somewhat) standard INI format - sections and keys are case insensitive, and any leading or trailing whitespace is ignored. Comments are lines that begin with a semicolon. Trailing comments are only allowed on section lines.

After reading, data is kept in memory. Files are read from or written to disk on demand and closed immediately after. This library supports lazy writing, which only writes changes and updates and preserves custom spacings and comments. A lazy write invoked by a `Write` call will read the output file, find changes made and update the file accordingly. If performance is a strong issue and you only need to generate files, use `Generate` instead.

Read and write order is preserved. New keys and sections will be written to the file in the same order they were added. Iterating through data will take the same order as the original file.

## Installation

Copy the folder in /src/ into your own project's source code folder, and add `ini.cpp` and `ini.h` to your project. If you're building manually, just make sure your compiler sees the source file. Then include the header file somewhere in your code:
```C++
#include "mini/ini.h"
```

You're good to go!

## Usage

### Instantiation

To create an instance:
```C++
mINI::INIFile ini;
```

### Reading from a file

Use `Read` to read from a file:
```C++
ini.Read("myfile.ini"); // returns true if successful
```

Alternatively, you can use the constructor to read from a file on instantiation:
```C++
mINI::INIFile ini("myfile.ini");
```

### Reading data

To read data, use one of the `Get` functions. If a value doesn't exist, default will be returned. Default values are either 0 or an empty string:
```C++
// retreive a string value
// section and key are string values
ini.Get(section, key); // default: empty string

// retreive a boolean value
// values "", "0", "no" and "false" map to false
// any other value maps to true
ini.GetBool(section, key); // default: false

// retreive a char value
ini.GetChar(section, key); // default: 0

// retreive an int value
ini.GetInt(section, key);

// retreive an unsigned int value
ini.GetUInt(section, key);

// retreive a double value
ini.GetDouble(section, key);
```

To get an entire section as a collection of key and value pairs:
```C++
// returns a pointer to INICollection or nullptr if section is not found
mINI::INICollection* keyValueStore = ini.Get(section);
// example usage
if (keyValueStore)
{
    for (auto const& it : *keyValueStore)
    {
	    std::string const& key = it.first;
	    std::string const& value = it.second;
	    std::cout << key << ": " << value << std::endl;
    }
}
```

To check if a key exists:
```C++
ini.Has(section, key); // returns true or false
```

To check if a section exists:
```C++
ini.Has(section); // returns true or false
```

To get the number of keys in a given section:
```C++
ini.Size(section); // returns a size_t, defaults to 0 if section is not found
```

To get the number of sections:
```C++
ini.Size(); // returns a size_t
```

### Updating data

To update old values or create new ones, use the `Set` function:
```C++
// section and key are string values
// value may be one of the following: string, bool, char, int, unsigned int, double
ini.Set(section, key, value);
```

To set multiple values for a single section at once:
```C++
ini.Set(section, {
	{key, value},
	{key, value},
	{key, value},
	...
});
```

To remove a single key:
```C++
ini.Remove(section, key); // returns true if remove was successful
```

To remove a section:
```C++
ini.Remove(section); // returns true if remove was successful
```

To clear all data:
```C++
ini.Clear();
```

### Writing to a file

To write back to a file we previously read while preserving spacings, comments, blank lines and other data:
```C++
// pretty parameter is optional. if set to true, spacings will be added around values and keys and
// blank lines will be added between sections
bool pretty = true;
ini.Write(pretty); // returns true if successful
// you can specify a filename if needed:
std::string filename = "test.ini";
ini.Write(filename, pretty);
```

To generate an INI file and overwrite any existing formatting, comments and other data from the original file:
```C++
std::string filename = "test.ini";
bool pretty = true;
ini.Generate(filename, pretty); // returns true if successful
// alternatively, you can generate back into original file by skipping the filename parameter
// use with caution as this will override any metadata, for example comments
ini.Generate(pretty);
```

### Iterating

To iterate through data in-order:
```C++
// loop through sections
for (auto const& it : ini)
{
	std::string const& section = it.first;
	mINI:INICollection const& collection = it.second;
	// output section name
	std::cout << '[' << section << ']' << std::endl;
	// loop through key/value pairs in each section
	for (auto const& it2 : collection)
	{
		std::string const& key = it2.first;
		std::string const& value = it2.second;
		// output key/value
		std::cout << key << ": " << value << std::endl;
	}
}
```

## Thanks

- [lest](https://github.com/martinmoene/lest) - testing framework

## License

Copyright (c) 2018 Danijel Durakovic

MIT License
