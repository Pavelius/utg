#include "nameable.h"
#include "variant.h"

#pragma once

struct advancei : nameable {
	char		level;
	variant		object;
	variants	elements;
};
