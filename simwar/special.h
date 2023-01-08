#include "statable.h"
#include "nameable.h"
#include "variant.h"

#pragma once

struct speciali {
	variant			object;
	const char*		group;
	cost_s			ability;
	int				multiplier, divider;
};