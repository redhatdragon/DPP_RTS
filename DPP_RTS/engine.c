#include "engine.h"
#include "IO_API/IO_API.h"
#include "JSON.h/json.h"
#include "math/math.h"
#include "mapper.h"
#include "math/math.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


struct json_object_element_s *jsonGetVariable(struct json_object_s *object, const char *variableName) {
	unsigned int length = object->length;
	for (unsigned int i = 0; i < length; i++) {
		struct json_object_element_s *element = object->start;
		for (unsigned int j = 0; j < i; j++) {
			element = element->next;
		}
		struct json_string_s *name = element->name;
		if (strcmp(name->string, variableName) == 0) {
			return element;
		}
	}
	return 0;
}

void loadLevel() {

}