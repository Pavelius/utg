#include "nameable.h"
#include "variant.h"

#pragma once

struct advancei : nameable {
	char		level, choose;
	variant		object;
	variants	elements;
};
extern variant last_result;
