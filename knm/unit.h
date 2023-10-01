#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct uniti : entity, abilitya {
	variants	combat;
};
extern uniti* last_unit;