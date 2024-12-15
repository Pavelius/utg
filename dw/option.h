#pragma once

#include "nameable.h"
#include "variant.h"

struct optioni : nameable {
	variant			parent;
	unsigned char	index;
	variants		use, condition;
};