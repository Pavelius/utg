#include "nameable.h"
#include "tag.h"

#pragma once

struct actioni : nameable {
	char		priority; // 0 - 10
	tagf		tags;
	char		conquer, explore, mobilize;
	bool		is(tag_s v) const { return tags.is(v); }
};
extern actioni* action;