#include "nameable.h"
#include "variant.h"

#pragma once

struct spelli : nameable {
	char		classes[8];
	char		rounds;
	variants	enchant, summon;
};