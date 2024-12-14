#pragma once

#include "nameable.h"
#include "variant.h"

struct optioni : nameable {
	variant parent;
	variants use, condition;
};