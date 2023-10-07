#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

enum deck_s : unsigned char;

struct cardi : entity, abilitya {
	const char*		trigger;
	deck_s			type;
	unsigned char	count;
	variants		effect, target;
	bool			usedeck() const { return count != 0; }
};
struct card : entity {
};
extern card* last_card;