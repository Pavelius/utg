#include "ability.h"
#include "answers.h"
#include "crt.h"
#include "draw.h"
#include "flagable.h"
#include "stringbuilder.h"
#include "tag.h"
#include "variant.h"

#pragma once

enum realm_s : unsigned char {
	Slash, Triangle, Hexagon, Circle, Star, Romb, Quad,
};
enum ability_s : unsigned char {
	Speed, Sneak, Fight, Will, Lore, Luck,
	Horror, Escape,
};
enum tag_s : unsigned char {
	Ambush, Endless, Undead,
	PhysicalResistance, PhysicalImmunity, MagicalResistance, MagicalImmunity,
	NightmarishI, NightmarishII, OverwhelmingI, OverwhelmingII,
	BonusVsUndead, ExhauseUse, NoSteal, SingleUse, Versatile,
};
enum trigger_s : unsigned char {
	NoTrigger, PhysicalWeapon, MagicalWeapon, Book,
	CombatCheck, EvadeCheck, HorrorCheck, SpellCheck,
	MovementPhase,
	HealthLose, SanityLose, HealthOrSanityLose,
};
enum cardtype_s : unsigned char {
	Ally, CommonItem, Spell, UniqueItem,
};

struct taga : flagable<8> {
};
struct abilitya {
	char			abilities[Escape + 1];
	void			add(const abilitya& e);
};
struct nameablei {
	const char*		id;
};
struct locationi : nameablei {
	point			position;
	locationi*		neightboard[4];
};
struct scripti {
	typedef void (*fnevent)(int counter, int param);
	typedef void (*fninfo)(stringbuilder& sb, const scripti& e, int counter);
	typedef bool (*fntest)(int counter, int param);
	const char*		id;
	fnevent			proc;
	int				param;
	fntest			choose;
	unsigned		flags;
	fninfo			info;
	void			getinfo(stringbuilder& sb, int bonus) const;
};
struct realmi {
	const char*		id;
};
struct indicatori {
	const char*		id;
};
struct cardtypei {
	const char*		id;
};
struct triggeri {
	const char*		id;
};
struct mythi : nameablei, abilitya {
	variant			gate, clue;
	short unsigned	white, black;
	variants		effect;
};
struct cardi : nameablei, abilitya {
	cardtype_s		type;
	trigger_s		trigger;
	char			hands, difficult, cost, count, pay, bonus;
	variants		effect;
	taga			tags;
};
struct monsteri : nameablei {
	realm_s			realm;
	char			count;
	char			toughness, alertness;
	char			horror_modifier, horror_damage, combat_modifier, combat_damage;
	taga			tags;
};
struct character : public abilitya {
};
struct gamei : abilitya {
};