#include "nameable.h"
#include "variant.h"

#pragma once

struct spelli : nameable {
	char		tier;
	unsigned	classes;
	char		rounds;
	variants	enchant, summon;
};