#include "nameable.h"
#include "stringbuilder.h"

#pragma once

struct strprinti : nameable {
	fnprint		proc;
};

bool print_identifier(stringbuilder& sb, const char* identifier);