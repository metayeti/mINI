# mINI

** STILL IN DEVELOPMENT - DON'T USE!!! **

This is a tiny C++ utility library for manipulating INI files.

It conforms to the following format:
- section and key names are case insensitive
- empty section or key names are ignored
- keys that do not belong to a section are ignored
- comments are lines that begin with a semicolon
- trailing comments are only allowed on section lines

Files are read on demand in one fell swoop, after which the data is kept in memory and is ready to be manipulated. Files are closed after read or write operations. This utility supports lazy writing, which only writes changes and updates and preserves custom spacings and comments. A lazy write invoked by a `write()` call will read the output file, find what changes have been made, and update the file accordingly. If performance is a strong issue and/or you only need to generate files, use `generate()` instead.

Section and key order is preserved on read and write operations. Iterating through data will take the same order as the original file.

This library operates with the `std::string` type to hold values and relies on your host environment for encoding.

## Installation

This is a header-only library. To install it, just copy everything in `/src/` into your own project's source code folder, or use a custom location and just make sure your compiler sees the additional include directory. Then include the file somewhere in your code:

```C++
#include "mini/ini.h"
```

You're good to go!

## Minimal example

Start with an INI file named `myfile.ini`:
```INI
; amounts of fruits
[fruits]
apples=20
oranges=30
```

Our code:
```C++
// first, create an INI file
mINI::INIFile file("myfile.ini");

// next, create a structure that will hold data
mINI::INIStructure ini;

// now we can read the file
file.read(ini);

// read a value
auto const& amountOfApples = ini["fruits"]["apples"];

// update a value
ini["fruits"]["oranges"] = "50";

// add a value
ini["fruits"]["bananas"] = "100";

// write updates to file
file.write(ini);
```

After running the code, our INI file now looks like this:
```INI
; amounts of fruits
[fruits]
apples=20
oranges=50
bananas=100
```

## Manipulating files

The `INIFile` class holds the filename and exposes functions for reading, writing and generating INI files.

To create a file instance:
```C++
mINI::INIFile file("myfile.ini");
```

You will also need a structure you can operate on:
```C++
mINI::INIStructure ini;
```

To read from a file:
```C++
bool readSuccess = file.read(ini);
```

To write back to a file while preserving comments and custom formatting:
```C++
bool writeSuccess = file.write(ini);
```

You can set the second parameter to `write()` to `true` if you want the file to be written with pretty-print. Pretty-print adds spaces between key-value pairs and blank lines between sections in the output file:
```C++
bool writeSuccess = file.write(ini, true);
```

To generate a file:
```C++
file.generate(ini);
```

Note that `generate()` will override any custom formatting and comments from the original file!

You can use pretty-print with `generate()` as well:
```C++
file.generate(ini, true);
```

Example: generated INI without pretty-print:
```INI
[section1]
key1=value1
key2=value2
[section2]
key1=value1
```

Example: generated INI with pretty-print:
```INI
[section1]
key1 = value1
key2 = value2

[section2]
key1 = value1
```

## Manipulating data

### Reading values

There are two ways of reading data from the INI structure. You can either use the `[]` operator or the `get()` function:

```C++
// read values. if key doesn't exist, it will be created
auto& value = ini["section"]["key"];

// read values safely - if key doesn't exist it will NOT be created
auto value = ini.get("section").get("key");
```

IMPORTANT: The difference between the `[]` and `get()` operations is that `[]` returns a reference to **real** data that you may modify and creates a new item automatically if it does not yet exist, while `get()` returns a **copy** of the data and does not create new keys. Use `has()` before doing any operations with `[]` if you don't wish to create new items. You may also want to avoid using `get()` whenever you're dealing with any sort of system constraints or enormous structures.

You can combine usage of `[]` and `get()` as your leisure:
```C++
// will get a copy of the section and retreive a key
// technically a better way to read data safely than .get().get() since it only
// copies data once; does not create new keys
ini.get("section")["key"];

// if we're sure section exists and we just want a copy of key if one exists
// without creating an empty value when the key doesn't exist:
ini["section"].get("key");

// you may chain other functions in a similar way
// the following code gets a copy of section and checks if a key exists:
ini.get("section").has("key");
```

### Updating values

To set or update a value:
```C++
ini["section"]["key"] = "value";
```

You can set multiple values at once by using `set()`:
```C++
ini["section"].set({
	{"key1", "value1"},
	{"key2", "value2"}
});
```

To remove a single key from a section:
```C++
bool removeSuccess = ini["section"].remove("key");
```

To remove a section:
```C++
bool removeSuccess = ini.remove("section");
```

To remove all keys from a section:
```C++
ini["section"].clear();
```

To remove all data in structure:
```C++
ini.clear();
```

### Other functions

To check if a section is present:
```C++
bool hasSection = ini.has("section");
```

To check if a key within a section is present:
```C++
bool hasKey = ini["section"].has("key");
```

To get the number of keys in a section:
```C++
size_t n_keys = ini["section"].size();
```

To get the number of sections in the structure:
```C++
size_t n_sections = ini.size();
```

IMPORTANT: Keep in mind that `[]` will always create a new item if one does not already exist! You can use `has()` to check if an item exists before performing further operations. Remember that `get()` will return a copy of data, so you should **not** do removes or updates to data with it. Straightforward usage of the `[]` operator shouldn't be a problem in most real-world cases where you're doing lookups on known keys and you may not care if empty keys or sections get created, but this is something to keep in mind when dealing with this datastructure. Always use `has()` before using the `[]` operator IF you don't want new empty sections and keys. Below is a short example that demonstrates safe manipulation of data.

```C++
if (ini.has("section"))
{
	// we have section, we can access it safely without creating a new one
	auto& collection = ini["section"];
	if (collection.has("key"))
	{
		// we have key, we can access it safely without creating a new one
		auto& value = collection["key"];
		// do something with value, for example change it
		value = "some value";
	}
}
```

### Iteration

To iterate through data in-order and display results:
```C++
for (auto const& it : ini)
{
	auto const& section = it.first();
	auto const& collection = *it.second();
	std::cout << "[" << section << "]" << std::endl;
	for (auto const& it2 : collection)
	{
		auto const& key = it2.first();
		auto const& value = *it2.second();
		std::cout << key << "=" << value << std::endl;
	}
}
```

## Thanks

- [lest](https://github.com/martinmoene/lest) - testing framework

## License

Copyright (c) 2018 Danijel Durakovic

MIT License
