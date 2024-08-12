#pragma once
#include "crt.h"
#include "nameable.h"
#include "point.h"

struct imagei : nameable {
	const char*	name;
	const char*	url;
	const char*	suffix;
	const char*	prefix;
	point		position, offset;
	void*		object;
};

struct imagea : imagei {
	sliceu<imagei> elements;
};
