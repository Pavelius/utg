#include "ability.h"
#include "entitya.h"

#pragma once

struct playeri;

struct armyi : abilitya {
	playeri*	player;
	entitya		troops;
	entitya		casualty;
	entitya		tactics;
	void		add(abilityn v, int i) { abilities[v] += i; }
	void		applycasualty();
	void		clear();
	void		damage(int chance, int count = 1);
	void		engage(abilityn type);
	void		engage(const char* id, int chance, int count);
	int			get(abilityn v) const { return abilities[v]; }
	int			getsummary(abilityn v) const;
	int			getstrenght() const { return getsummary(Strenght); }
	void		prepare(abilityn type);
	void		resist(int& hits);
	static void setheader(const char* format);
	void		select(const entity* location, playeri* player);
	void		suffer(int hits, bool shield_save = true);
};
extern armyi	attacker, defender;
extern armyi	*last_army, *winner_army;