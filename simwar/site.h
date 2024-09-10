#include "province.h"
#include "variant.h"

#pragma once

struct sitei : nameable {
	const char*		resid;
	costa			effect, upkeep, cost;
};
struct site {
	const sitei*	type;
	provincei*		province;
	costa			current;
	explicit operator bool() const { return type != 0; }
	void			clear();
};
extern sitei* lastsite;
extern site* location;
