#include "variant.h"

#pragma once

struct menu {
	const char*		id;
	const char*		mode;
	const char*		format;
	const varianti*	source;
	variants		elements;
	bool			isvisible() const;
	static const menu* last;
	static const char* current_mode;
};
