#include "nameable.h"

#pragma once

enum costn : unsigned char {
	Resources, Influence, Gold, Happiness, Fame, Lore, Units,
	Strenght, Sword, Shield, Death,
	Movement, Sail, Explore,
	Recruit, Size,
	Limit,
};
struct costi : nameable {
	int			frame = -1;
	int			frame_negative = -1;
};
typedef short costa[Limit + 1];

void addvalue(costa& v1, const costa& v2);
bool isenought(const costa& v1, const costa& v2);
bool isempthy(const costa& v);
void subvalue(costa& v1, const costa& v2);