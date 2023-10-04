#include "entity.h"
#include "variant.h"

#pragma once

enum component_s : unsigned char {
	ActionCard, CombatCard, PlayerAction,
};
struct cardi : entity {
	component_s		type;
	unsigned char	count;
	variants		effect;
};