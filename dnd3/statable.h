#include "flagux.h"

#pragma once

enum abilitybonus_s : unsigned char {
	DiceD3, DiceD4, DiceD6, DiceD8,
	WeaponDice,
};

struct statable {
	char		abilities[64];
	char		bonus[WeaponDice + 1];
	unsigned	skills, skills_expert;
	unsigned	immunity, resistance, vulnerable;
	unsigned	proficiency, proficiency_expert;
	flagux<64>	flags;
	void		creating();
	int			getability(const char* id) const;
	void		copy(const statable& v);
	bool		isimmunity(int material) const;
	bool		isresistance(int material) const;
	bool		istag(int tag) const { return flags.is(tag); }
	bool		isvulnerable(int material) const;
	void		setability(const char* id, int value);
	void		update();
};
extern int ability_result;
extern int result_param[4];