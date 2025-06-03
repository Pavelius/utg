#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct uniti : entity, abilitya {
	variants	combat;
	int			get(abilityn v) const { return abilities[v]; }
};
extern uniti* last_unit;