#include "ability.h"
#include "move.h"

#pragma once

struct statable {
	movea	forward;
	char	forward_any;
	char	abilities[Might + 1];
	int		get(abilityn v) const { return abilities[v]; }
};