#include "crt.h"
#include "stringbuilder.h"

#pragma once

struct costitem {
	const char*		id;
	int				value;
};
struct costitema : adat<costitem, 32> {
};
extern costitema* lastcostitem;

void add_cost_items(stringbuilder& sb);
int get_value(const char* id, int value);