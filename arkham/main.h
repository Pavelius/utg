#include "ability.h"
#include "answers.h"
#include "crt.h"
#include "draw.h"
#include "flagable.h"
#include "quest.h"
#include "stringbuilder.h"
#include "tag.h"
#include "variant.h"

#pragma once

typedef short unsigned cardt;

enum realm_s : unsigned char {
	Slash, Triangle, Hexagon, Circle, Star, Romb, Quad,
};
enum ability_s : unsigned char {
	Speed, Sneak, Fight, Will, Lore, Luck,
	Horror, Combat, Escape, HorrorDamage, CombatDamage, Toughness,
	Health, Sanity, Clue, Money,
	Focus,
};
enum tag_s : unsigned char {
	Ambush, Endless, Undead,
	PhysicalResistance, PhysicalImmunity, MagicalResistance, MagicalImmunity,
	NightmarishI, NightmarishII, OverwhelmingI, OverwhelmingII,
	BonusVsUndead, Exhause, NoSteal, Discard, Versatile,
};
enum trigger_s : unsigned char {
	NoTrigger, PhysicalWeapon, MagicalWeapon, Book,
	CombatCheck, EvadeCheck, HorrorCheck, SpellCheck,
	MovementPhase,
	HealthLose, SanityLose, HealthOrSanityLose,
};
enum cardtype_s : unsigned char {
	Ally, Arkham, CommonItem, Gate, Monster, Myth, OtherWorld, Skill, Special, Spell, Street, UniqueItem,
};
typedef adat<quest*> questa;
typedef slice<quest> quests;
typedef flagable<1> realma;
typedef flagable<8> taga;
typedef flagable<1> abilityf;
typedef flagable<2> cardf;
struct abilitya {
	char			abilities[Focus + 1];
	abilityf		rerollall, doubleclue, tought, restore;
	cardf			pickextra, scavenge;
	void			add(ability_s v) { abilities[v] += 1; }
	void			add(ability_s v, int i) { abilities[v] += i; }
	void			add(const abilitya& e);
	constexpr int	get(ability_s v) const { return abilities[v]; }
	bool			isdoubleclue(ability_s v) const { return doubleclue.is(v); }
	bool			ispickextra(cardtype_s v) const { return pickextra.is(v); }
	bool			isrestore(ability_s v) const { return restore.is(v); }
	bool			isrerollall(ability_s v) const { return rerollall.is(v); }
	bool			istought(ability_s v) const { return tought.is(v); }
	void			set(ability_s v, int i) { abilities[v] = i; }
};
struct nameablei {
	const char*		id;
};
struct locationi : nameablei {
	cardtype_s		type;
	point			position;
	locationi*		neightboard[4];
	quests			encounters;
	const quest*	choose(int count = 1) const;
	void			encounter(int count = 1) const;
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
struct deck : adat<cardt> {
	void			create(cardtype_s type);
	void			drop(cardt v);
	bool			havecard(cardt v) const;
	cardt			pick();
	bool			pick(cardt v);
	void			shuffle();
};
struct cardtypei {
	const char*		id;
	deck			cards;
};
struct triggeri {
	const char*		id;
};
struct cardprotoi : nameablei, abilitya {
	cardtype_s		type;
	trigger_s		trigger;
	char			hands, difficult, cost, count, pay, bonus;
	variants		effect;
	realma			realms;
	taga			tags;
	int				getcost(int discount) const;
};
struct cardi {
	short unsigned	type;
	unsigned char	exhaused : 1;
	unsigned char	uses : 3;
	constexpr explicit operator bool() const { return type != 0; }
	void			clear();
	void			discard();
	cardprotoi&		geti() const { return bsdata<cardprotoi>::elements[type]; }
	bool			isactive() const { return !exhaused; }
};
struct cardpool {
	adat<cardi, 32> source;
	void			addcard(cardt v);
	void			discard();
	bool			havecard(cardt v) const;
	bool			isdoubleclue(ability_s v) const;
	bool			isrerollall(ability_s v) const;
	void			pick(cardtype_s type);
	void			pick(cardtype_s type, int count);
};
struct investigator : nameablei, abilitya {
	variants		extra;
	locationi*		location;
	explicit operator bool() const { return location != 0; }
};
struct player : abilitya, cardpool {
	abilitya		maximum;
	static player*	last;
	locationi*		location;
	investigator*	prototype;
	void			create(const char* id);
	void			clear();
	void			encounter();
	void			delayed();
	int				getsuccess() const;
	int				getmaximal(ability_s v) const;
	int				getminimal(ability_s v) const;
	bool			isallowreroll(ability_s v) const;
	void			leavestreet();
	void			losehalf(cardtype_s m);
	void			movement(locationi* pv);
	int				roll(ability_s v, int m);
	void			update();
};
struct gamei : public player {
};
extern gamei		game;
extern answers		an;