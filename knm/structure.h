#include "ability.h"
#include "entity.h"

#pragma once

struct structurei : entity, abilitya {
	char		avatar;
	int			get(abilityn v) const { return abilities[v]; }
};
struct structure : entity {
};
extern structurei* last_structure;