#include "entity.h"
#include "variant.h"

#pragma once

struct cardi : entity {
	const char*		trigger;
	unsigned char	count;
	variants		effect, wearing;
	bool			usedeck() const { return count != 0; }
};
extern cardi* last_card;