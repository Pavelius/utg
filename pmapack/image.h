#pragma once
#include "crt.h"
#include "nameable.h"

struct imagei : nameable {
	const char*		name;
	const char*		url;
	const char*		suffix;
	const char*		prefix;
	int				x, y, sx, sy;
	void*			object;
};

struct imagea : nameable {
	sliceu<imagei>	elements;
};
