#include "province.h"

#pragma once

struct moveorder {
	unsigned short	from, to;
	int				count;
};
void add_move(const provincei* from, const provincei* to, int count);
