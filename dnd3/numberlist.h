#include "crt.h"
#include "nameable.h"

#pragma once

struct numberlist : nameable {
	slice<int>		elements;
	static void		read(const char* url);
};
