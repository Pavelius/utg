#include "creature.h"
#include "reaction.h"
#include "timeable.h"

#pragma once

struct scene {
	creature*		choosetarget() const;
	void			encounter(const monsteri* p);
	void			rollinitiative();
};
struct gamei : timeable, scene {
	reaction_s		reaction;
	bool			party_surprised, monster_surprised;
	void			initialize();
	static void		mainmenu();
	void			rollreaction(int bonus);
	void			rollsurprise();
};
extern gamei		game;
void				combat_mode();
