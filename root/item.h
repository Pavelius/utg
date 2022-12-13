#include "nameable.h"
#include "tagable.h"

#pragma once

struct itemi : nameable, tagable {
	char wear;
	constexpr bool isweapon() const { return is(Far) || is(Close) || is(Intimate); }
};
struct item {
	unsigned char type;
	unsigned char wear : 3;
	unsigned char wear_max : 3;
	constexpr explicit operator bool() const { return type != 0; }
	const itemi& geti() const { return bsdata<itemi>::elements[type]; }
};