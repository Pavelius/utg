#include "province.h"
#include "variant.h"

#pragma once

struct sitei : nameable {
	const char*		resid;
	costa			effect, upkeep, cost;
	variants		conditions;
	variants		fail, partial, success;
	bool			isallow(variant v) const;
	bool			isallow() const;
};
struct site {
	const sitei*	type;
	provincei*		province;
	explicit operator bool() const { return type != 0; }
	void			clear();
};
extern sitei* lastsite;
extern site* location;
