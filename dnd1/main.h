#include "dice.h"
#include "gender.h"
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
enum feat_s : unsigned char {
	SunSensitive,
	Undead, Summoned, Player, Enemy,
};
enum class_s : unsigned char {
	Monster,
	Cleric, Dwarf, Elf, Fighter, Halfling, Theif, Wizard,
};
enum duration_s : unsigned char {
	Instant,
	Round,
	Turn, Turn2, Turn3, Turn2d6, Turn4d4,
	Hour, Hour2,
	Permanent,
};
enum range_s : unsigned char {
	Caster, CasterOrCreatureTouched,
	Range5, Range10, Range30, Range60, Range120, Range150, Range240,
};
enum reaction_s : unsigned char {
	Hostile, Unfriendly, Neutral, Indifferent, Friendly
};
enum spell_s : unsigned char {
	CureLightWound, DetectEvil, DetectMagic, Light, ProtectionFromEvil, PurifyFoodAndWater, RemoveFear, ResistCold,
	CharmPerson, FloatingDisc, HoldPortal, MagicMissile, ReadLanguages, ReadMagic, Shield, Sleep, Ventriloquism
};
inline int d6() { return 1 + rand() % 6; }
struct abilityi : nameable {
};
struct featable : flagable<4> {};
struct rangei : nameable {
};
struct actable {
	const char*		name;
	gender_s		gender;
	void			act(const char* format, ...) const { actv(*answers::console, format, xva_start(format)); }
	void			actv(stringbuilder& sb, const char* format, const char* format_param) const;
	const char*		getname() const { return name; }
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
struct feati : nameable {
};
struct chooseoption {
	typedef bool(*fntest)();
	const char*		id;
	fnevent			proc;
	fntest			test;
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
	char			level[3];
	duration_s		duration;
	range_s			range;
	dice			effect;
};
struct spellable {
	unsigned char	spells[Ventriloquism + 1];
};
struct item {
	unsigned char	type, subtype;
	unsigned char	cursed : 1;
};
struct creature : actable, spellable, statable {
	class_s			type;
	statable		basic;
	featable		feats;
	char			initiative;
	unsigned		experience;
	static creature* last;
	bool			attack(ability_s attack, int ac, int bonus) const;
	void			chooseoptions();
	void			clear() { memset(this, 0, sizeof(*this)); }
	void			create(class_s type, gender_s gender);
	void			damage(int value);
	int				getbonus(ability_s v) const;
	feat_s			getenemyfeat() const;
	int				gethit() const;
	bool			is(spell_s v) const { return spells[v]!=0; }
	bool			is(feat_s v) const { return feats.is(v); }
	bool			isactive(spell_s v) const;
	void			levelup();
	void			raiselevel();
	static const char* randomname(class_s type, gender_s gender);
	void			set(feat_s v) { feats.set(v); }
	void			update();
};
struct creaturea : adat<creature*, 32> {
	void			match(feat_s v, bool keep);
	void			select();
};
struct monsteri : nameable, statable {
	featable		feats;
};
struct scene {
	creaturea		creatures;
	static creaturea targets;
	creature*		choosetarget() const;
	void			rollinitiative();
};
struct gamei : scene {
	reaction_s		reaction;
	bool			party_surprised, monster_surprised;
	static void		combatmode();
	void			initialize();
	static void		mainmenu();
	void			rollreaction(int bonus);
	void			rollsurprise();
};
extern gamei game;
struct ongoing {
	spell_s			id;
	creature*		owner;
	unsigned		duration;
	static ongoing*	find(spell_s id, const creature* p);
};