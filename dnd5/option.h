#include "ability.h"
#include "nameable.h"
#include "spell.h"
#include "variant.h"

#pragma once

struct optioni : nameable {
	const abilityi*	parent;
	const spelli*	spells[10];
	variants		effect;
};