#pragma once

#include "collection.h"
#include "nameable.h"
#include "placeable.h"
#include "typeable.h"

struct areai : nameable {
};
struct area : typeable<areai> {
	void	addlink(const area* to);
};
extern collection<area> areas;
extern area* last_area;

void add_area(const char* id);