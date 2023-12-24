#include "quest.h"

#pragma once

struct questlist {
	const char*		id;
	sliceu<quest>	elements;
	const quest*	find(int index, const quest* start = 0) const;
	static void		read(const char* id);
};
extern questlist* last_questlist;

void questlist_initialize();