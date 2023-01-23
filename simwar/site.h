#include "province.h"
#include "variant.h"

#pragma once

struct sitei : nameable {
	const char*		resid;
	costac			effect, upkeep;
	costa			cost;
	variants		conditions;
	bool			isallow(variant v) const;
	bool			isallow() const;
};
struct site {
	const sitei*	type;
	provincei*		province;
	tag_s			tags;
	explicit operator bool() const { return type != 0; }
	void			clear();
};
extern sitei* lastsite;
