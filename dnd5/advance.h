#pragma once
#include "nameable.h"
#include "variant.h"

struct advancei : nameable {
	char		level, choose;
	variant		object;
	variants	elements;
};
