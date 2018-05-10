# mINI

** STILL IN DEVELOPMENT - DON'T USE!!! **

This is a tiny C++ utility library for manipulating INI files.

It conforms to the following format:
- section and key names are case insensitive
- empty section or key names are ignored
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

Our INI file now looks like this:
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

Note that `generate` will override any custom formatting and comments from the original file.

You can use the pretty-print with `generate()` as well:
```C++
file.generate(ini, true);
```

Example - generated INI without pretty-print:
```INI
[section1]
key1=value1
key2=value2
[section2]
key1=value1
```

Example - generated INI with pretty-print:
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
auto value = ini["section"]["key"];

// read values safely. if key doesn't exist it will NOT be created
auto value = ini.get("section").get("key");
```

IMPORTANT: The difference between the `[]` and `get()` operations is that `[]` returns a reference to *real* data that you may modify and creates a new item automatically if it does not yet exist, while `get()` returns a *copy* of the data and does not create new keys. Use `has()` before doing any operations with `[]` if you don't wish to create new items. You may also want to avoid using `get()` whenever you're dealing with any sort of system constraints or enormous structures.

### Updating values

To check if a section is present:
```C++
bool hasSection = ini.has("section");
```

## Thanks

- [lest](https://github.com/martinmoene/lest) - testing framework

## License

Copyright (c) 2018 Danijel Durakovic

MIT License