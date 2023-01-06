#include "province.h"

#pragma once

struct buildingi : nameable {
	const buildingi*	upgrade;
	costa				effect, upkeep, cost;
};
struct building : moveable {
	const buildingi*	type;
	explicit operator bool() const { return type != 0; }
};
extern buildingi*		lastbuilding;