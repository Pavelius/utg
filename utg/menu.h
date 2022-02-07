#include "variant.h"

#pragma once

struct menu {
	const char*		id;
	const char*		mode;
	const varianti*	source;
	variants		elements;
	bool			isvisible() const;
	static const menu* last;
	static const char* current_mode;
};
