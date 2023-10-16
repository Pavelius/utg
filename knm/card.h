#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct decki;

struct cardi : entity, abilitya {
	const char*		trigger;
	char			count, priority;
	variants		effect, target;
	decki*			getdeck() const { return (decki*)location; }
	void			play();
	bool			usedeck() const { return count != 0; }
};
struct card : entity {
	cardi*			getcomponent() const { return (cardi*)id; }
	void			discard(bool to_the_top = false);
	void			play();
};
extern cardi* last_component;
extern card* last_card;