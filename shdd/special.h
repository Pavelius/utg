#include "ability.h"
#include "variant.h"

#pragma once

struct speciali : nameable {
	ability_s	save;
	int			dc;
	variant		effect;
};