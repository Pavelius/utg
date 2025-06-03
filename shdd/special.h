#include "ability.h"
#include "variant.h"

#pragma once

struct speciali : nameable {
	abilityn	save;
	int			dc;
	variant		effect;
};