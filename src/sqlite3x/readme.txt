The sqlite3x wrappers are very small and are meant to be compiled in with your
project.  Have your project link to the sqlite dll.

Caveat: The code does assumes wchar_t is a 2-byte UTF-16 data type, so Unicode
support isn't likely to work on *nix (which defines wchar_t as UTF-32).