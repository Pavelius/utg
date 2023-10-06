#include "ability.h"
#include "entity.h"

#pragma once

struct structurei : entity, abilitya {
	int			get(ability_s v) const { return abilities[v]; }
};
struct structure : entity {
};