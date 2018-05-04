# mINI

**This project is still under development!**

This is a tiny utility library for reading from and writing data to INI files with a straightforward API and a minimal footprint. It conforms to the (somewhat) standard INI format - sections and keys are case insensitive, and any leading or trailing whitespace is ignored. Comments are lines that begin with a semicolon. Trailing comments are disallowed since values may also contain semicolons.

This library supports lazy writing, which only writes changes and updates and preserves custom spacings and comments. A lazy write invoked by a `Write` call will read the output file, find changes made and update the file accordingly. If performance is a strong issue and you only need to generate files, use `Generate` instead.

Read and write order is preserved. New keys and sections will be written to the file in the same order they were added. Iterating through data will take the same order as the original file.

## Installation

Copy the folder in /src/ into your own project's source code folder, and add `ini.cpp` and `ini.h` to your project. Then include the header file somewhere in your code:
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
ini.Read("myfile.ini");
```

Alternatively, use the constructor to read from a file on instantiation:
```C++
mINI::INIFile ini("myfile.ini");
```

### Reading data

To read data, use one of the `Get` functions:
```C++
// retreive a string value
// section and key are string values
ini.Get(section, key);

// retreive a boolean value
// values "", "0", "no" and "false" map to false
// any other value maps to true
ini.GetBool(section, key);

// retreive an int value
ini.GetInt(section, key);

// retreive an unsigned int value
ini.GetUInt(section, key);

// retreive a float value
ini.GetFloat(section, key);

// retreive a double value
ini.GetDouble(section, key);
```

To check if a key exists:
```C++
ini.Has(section, key);
```

To check if a section exists:
```C++
ini.Has(section);
```

### Updating data

To update old values or create new ones, use the `Set` function:
```C++
// value may be one of the following: string, bool, char, int, unsigned int, float, double
ini.Set(section, key, value);
```

To remove a single key:
```C++
ini.Remove(section, key);
```

To remove a section:
```C++
ini.Remove(section);
```

To clear all data:
```C++
ini.Clear();
```

### Writing to file

To generate an INI file and overwrite any existing formatting, comments and other data from the original file:
```C++
// the pretty parameter is optional
// if set to true, spacings will be added between values and keys and blank lines
// will be added between sections
bool pretty = true;
ini.Generate("myfile.ini", pretty);
```

To write back to a file while preserving spacings, comments, blank lines and other data:
```C++
bool pretty = true;
ini.Write(pretty);
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