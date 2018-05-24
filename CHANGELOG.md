## 0.9.3 (May 24, 2018)
- `BUGFIX` Fixes inconsistent behavior with empty section and key names where read would ignore empty names and write would allow them.
- `FEATURE` Empty key and section names are now allowed.

## 0.9.2 (May 24, 2018)
- `BUGFIX` Fixes the multiple definition bug [#1](/../../issues/1)
- `BUGFIX` Fixes a bug where a `write()` call to an empty file would begin writing at line 2 instead of 1 due to a reader bug.

## 0.9.1 (May 20, 2018)
- `BUGFIX` Fixed a bug where the writer would skip writing new keys and values following an empty section.

## 0.9.0 (May 20, 2018)
- Release v0.9.0
