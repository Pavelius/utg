#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct uniti : entity, abilitya {
	variants	combat;
	int			get(ability_s v) const { return abilities[v]; }
};
extern uniti* last_unit;