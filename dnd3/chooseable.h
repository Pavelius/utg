#include "nameable.h"
#include "variant.h"

#pragma once

struct chooseablei : nameable {
	variant		type;
};
struct chooseable {
	short		values[32];
};
