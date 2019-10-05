#include "mapper.h"
#include <string.h>

char key[256][256];
unsigned int keyCount = 1;

unsigned int mapString(const char *str) {
	strcpy(&key[keyCount][0], str);
	keyCount++;
}
unsigned int mapStringSafe(const char* str) {
	unsigned int key = mapGetKeyFromString(str);
	if (!key) {
		return mapString(str);
	}
	return key;
}
unsigned int mapGetKeyFromString(const char *str) {
	unsigned int count = keyCount;
	for (unsigned int i = 1; i < count; i++) {
		if (strcmp(&key[i][0], str) == 0)
			return i;
	}
	return 0;
}
const char *mapGetStringFromKey(unsigned int uid) {
	return &key[uid][0];
}