# mINI

This is a tiny C++ utility library for manipulating with INI files.

It conforms to the following format:
- section and keys are case insensitive
- empty section or key names are ignored
- comments are lines that begin with a semicolon
- trailing comments are only allowed on section lines

Files are read on demand in one fell swoop, after which the data is kept in memory and is ready to be manipulated. Files are closed after read or write operations. This utility supports lazy writing, which only writes changes and updates and preserves custom spacings and comments. A lazy write invoked by a `write()` call will read the output file, find what changes have been made, and update the file accordingly. If performance is a strong issue and/or you only need to generate files, use `generate()` instead.

Section and key order is preserved on read and write operations. Iterating through data will take the same order as the original file.

This library operates with `std::string` type to hold values and relies on your host environment for encoding.

## Installation

This is a header-only library. To install it, just copy everything in `/src/` into your own project's source code folder, or use a custom location and just make sure your compiler sees the additional include directory. Then include the file somewhere in your code:

```C++
#include "mini/ini.h"
```

You're good to go!

## Minimal example

Start with a basic INI file named `myfile.ini`:
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

## Thanks

- [lest](https://github.com/martinmoene/lest) - testing framework

## License

Copyright (c) 2018 Danijel Durakovic

MIT License