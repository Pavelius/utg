#include "site.h"

#pragma once

struct buildingi : sitei {
	const buildingi*	upgrade;
	costa				cost;
};
struct building {
	const buildingi*	type;
	provincei*			province;
	explicit operator bool() const { return type != 0; }
	void				clear();
};
extern buildingi* lastbuilding;