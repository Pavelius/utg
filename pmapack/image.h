#pragma once

#include "crt.h"
#include "nameable.h"
#include "point.h"

struct imagei : nameable {
	const char*	url;
	const char*	name;
	const char*	suffix;
	const char*	prefix;
	point		position, center;
	int			count;
	void*		object;
};
struct imagea : imagei {
	sliceu<imagei> elements;
};

void image_read(const char* url);
