#pragma once

#include "nameable.h"
#include "variant.h"

struct actioni : nameable {
	variants	effect, filter;
};
