#include "nameable.h"

#pragma once

struct variable : nameable {
	int			value, maximum;
};
extern variable* last_variable;