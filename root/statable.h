#include "ability.h"
#include "move.h"

#pragma once

struct statable {
	movea	forward;
	char	forward_any;
	char	abilities[Might + 1];
	int		get(ability_s v) const { return abilities[v]; }
};