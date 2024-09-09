#include "nameable.h"

#pragma once

enum costn : unsigned char {
	Resources, Influence, Gold, Happiness, Fame, Lore, Warfire,
	Strenght, Sword, Shield, Death,
	Movement, Sail,
	Recruit, Size,
	Limit,
};
struct costi : nameable {
	const char*	format;
	int			frame = -1;
};
typedef short costa[Limit + 1];

void addvalue(costa& v1, const costa& v2);
bool isenought(const costa& v1, const costa& v2);
bool isempthy(const costa& v);
void subvalue(costa& v1, const costa& v2);