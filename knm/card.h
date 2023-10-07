#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct cardi : entity, abilitya {
	const char*		trigger;
	unsigned char	count;
	variants		effect, target;
	bool			usedeck() const { return count != 0; }
};
struct card : entity {
};
extern card* last_card;