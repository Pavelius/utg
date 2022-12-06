#include "nameable.h"
#include "spell.h"

#pragma once

struct divinedomain : nameable {
	const spelli*	spells[10];
	variants		bonus;
};
