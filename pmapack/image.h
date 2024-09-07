#pragma once

#include "nameable.h"
#include "point.h"
#include "sliceu.h"

const int DefaultImageNumber = -1000;

struct imagei : nameable {
	const char*	url;
	const char*	name;
	const char*	ext;
	const char*	dest_url;
	point		position, size, center;
	int			param, bpp;
	void*		object;
};
extern imagei last_image;
struct imagea : imagei {
	sliceu<imagei> elements;
};
struct imageplugini : nameable {
	fnevent		proc;
	const char*	description;
};

void image_read(const char* url);
void image_run(const char* id);

const char* image_source_url();
const char* image_dest_url(const char* ext = 0);
