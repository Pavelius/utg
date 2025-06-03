#include "nameable.h"
#include "variant.h"

#pragma once

struct enumgroupi : nameable {
};
struct enumi : nameable {
	variant	type;
	int		index;
};
struct enumerable {
	short	values[32];
};
void initialize_enumerators();