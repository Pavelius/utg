#include "nameable.h"
#include "variant.h"

#pragma once

struct enumgroupi : nameable {
};
struct enumi : nameable {
	variant			type;
	int				index;
	static void		initialize();
};
struct enumerable {
	short			values[32];
};