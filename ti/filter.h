#pragma once

#include "nameable.h"

typedef bool(*fnvisible)(const void* object);

struct filteri : nameable {
	typedef void(*fncustom)(int bonus);
	fnvisible		proc;
	fncustom		custom;
	int				param;
};
extern filteri* last_filter;