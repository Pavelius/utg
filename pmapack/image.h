#pragma once

#include "crt.h"
#include "nameable.h"
#include "point.h"

typedef void(*fnimagemsg)(const char* format, const char* format_param);

struct imagei : nameable {
	const char*	url;
	const char*	name;
	const char*	ext;
	const char*	dest_url;
	point		position, size, center;
	int			count, bpp;
	void*		object;
};
struct imagea : imagei {
	sliceu<imagei> elements;
};
struct imageplugini : nameable {
	fnevent		proc;
};
extern imagei last_image;
extern fnimagemsg image_errorv_proc, image_messagev_proc;

void image_read(const char* url);
void image_run(const char* id);

const char* image_source_url();
const char* image_dest_url();
