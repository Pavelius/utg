#include "feat.h"
#include "statable.h"

#pragma once

struct provincei;

struct sitei : nameable, featable {
	const char*		resid;
	int				avatar;
	costa			income, upkeep, cost;
};
struct site {
	const sitei*	type;
	provincei*		province;
	char			explore;
	explicit operator bool() const { return type != 0; }
	void			clear();
	bool			is(featn v) const { return type->is(v); }
};
extern const sitei* last_site;
extern site* location;
