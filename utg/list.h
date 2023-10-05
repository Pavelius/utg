#include "nameable.h"
#include "variant.h"

#pragma once

struct listi : nameable {
	variants elements;
};
extern listi* last_list;