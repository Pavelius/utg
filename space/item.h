#include "nameable.h"

#pragma once

struct itemi : nameable {
	short			weight, cost;
};
struct item {
	unsigned short	type;
	unsigned char	modification, uses;
};