#pragma once
#include "quest.h"

struct questlist {
	const char*		id;
	sliceu<quest>	elements;
	const quest*	find(int index, const quest* start = 0) const;
};
extern questlist*	last_questlist;

const questlist* find_quest(const quest* p);

const quest* questlist_find_prompt(short id);

void questlist_read(const char* id);