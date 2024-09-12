#pragma once

#include "province.h"

struct playeri;

struct moveorder {
	unsigned short	from, to;
	playeri*		player;
	int				count;
};
void add_move(const provincei* from, const provincei* to, playeri* player, int count);
