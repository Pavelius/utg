#include "dice.h"
#include "nameable.h"
#include "utg.h"

#pragma once

enum ability_s : unsigned char {
	Strenght, Dexterity, Constitution, Intellect, Wisdow, Charisma,
	ToHit, Damage, MeleeToHit, MeleeDamage, RangedToHit, RangedDamage,
	AC, Level,
	HP, HPMax,
	SaveDeath, SaveWands, SaveParalize, SaveBreathWeapon, SaveSpells, SavePoison,
};
enum class_s : unsigned char {
	Monster,
	Cleric, Dwarf, Elf, Fighter, Halfling, Wizard,
};
enum duration_s : unsigned char {
	Round,
	Turn,
	Hour,
};
enum reaction_s : unsigned char {
	Hostile, Unfriendly, Neutral, Indifferent, Friendly
};
inline int d6() { return 1 + rand() % 6; }
struct abilityi : nameable {
};
struct rangei : nameable {
};
struct statable {
	char			abilities[SaveSpells + 1];
	void			add(ability_s i, int v) { abilities[i] += v; }
	int				get(ability_s i) const { return abilities[i]; }
};
struct classi : nameable {
	int				tohit, hd;
};
struct durationi : nameable {
};
struct itemi : nameable {
	struct weaponi {
		dice		damage;
	};
	struct armori {
		char		ac;
	};
	armori			armor;
	weaponi			weapon;
};
struct spelli : nameable {
	dice			damage;
	dice			damage_per_level;
	char			level_progress;
	duration_s		duration;
	int				range;
};
struct item {
	unsigned char	type, subtype;
	unsigned char	cursed : 1;
};
struct creature : statable {
	class_s			type;
	statable		basic;
	unsigned		experience;
	bool			attack(int bonus, int ac) const;
	void			damage(int value);
	int				gethit() const;
	void			levelup();
	void			raiselevel();
	void			update();
};
struct monsteri : nameable, statable {
};
struct gamei {
	reaction_s		reaction;
	bool			party_surprised, monster_surprised;
	void			rollreaction(int bonus);
	void			rollsurprise();
};