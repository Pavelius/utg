#pragma once

#include "entity.h"
#include "variant.h"

struct decki;

struct componenti : entity {
	variants		use;
	const char*		trigger;
	char			count;
	bool			isallow() const;
};
struct card : entity {
};
