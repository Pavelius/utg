#include "variant.h"

#pragma once

struct menu {
	const char*		id;
	const varianti*	source;
	variants		elements;
	static int		last_index;
};
