#pragma once

// Map and return unique identifier from string.
// Does not check for duplicate strings.
unsigned int mapString(const char *str);
// Map and return unique identifier from string.
// Map only if no duplicate exists.
unsigned int mapStringSafe(const char* str);
// Return unique unique identifier from string.
// If none exists return 0.
unsigned int mapGetKeyFromString(const char *str);
// Return string from unique identifier.
// Assumes unique identifier is valid.
const char *mapGetStringFromKey(unsigned int uid);