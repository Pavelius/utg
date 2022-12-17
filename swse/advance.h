#pragma once

#include "nameable.h"
#include "variant.h"

struct advancei : nameable {
	char		level;
	variant		object;
	variants	elements;
};
