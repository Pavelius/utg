#pragma once

#include "collection.h"
#include "nameable.h"
#include "placeable.h"
#include "typeable.h"

enum arean : unsigned short;
struct areai : nameable {
};
extern arean last_area_type;

struct area : typeable<areai> {
	void addlink(const area* to);
};
extern area* last_area;

void create_area(int bonus);