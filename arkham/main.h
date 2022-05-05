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
	BonusVsUndead, ExhauseUse, NoSteal, SingleUse, Versatile,
};
enum trigger_s : unsigned char {
	NoTrigger, PhysicalWeapon, MagicalWeapon, Book,
	CombatCheck, EvadeCheck, HorrorCheck, SpellCheck,
	MovementPhase,
	HealthLose, SanityLose, HealthOrSanityLose,
};
enum cardtype_s : unsigned char {
	Ally, Arkham, CommonItem, Monster, Myth, OtherWorld, Skill, Special, Spell, Street, UniqueItem,
};
typedef adat<quest*> questa;
typedef slice<quest> quests;
struct realma : flagable<1> {
};
struct taga : flagable<8> {
};
struct abilityf : flagable<1> {
};
struct abilitya {
	char			abilities[Focus + 1];
	abilityf		rerollall, doubleclue;
	void			add(ability_s v) { abilities[v] += 1; }
	void			add(ability_s v, int i) { abilities[v] += i; }
	void			add(const abilitya& e);
	constexpr int	get(ability_s v) const { return abilities[v]; }
	bool			isdoubleclue(ability_s v) const { return doubleclue.is(v); }
	bool			isrerollall(ability_s v) const { return rerollall.is(v); }
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
	static locationi* find(const char* id);
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
class deck : adat<short unsigned> {
public:
	void			create(cardtype_s type);
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
};
struct cardi {
	short unsigned	type;
	unsigned char	exhaused : 1;
	unsigned char	uses : 3;
	cardprotoi&		geti() const { return bsdata<cardprotoi>::elements[type]; }
	bool			isactive() const { return !exhaused; }
};
class cardpool {
	adat<cardi, 32>	source;
public:
	void			drop(cardt card);
	bool			isdoubleclue(ability_s v) const;
	bool			isrerollall(ability_s v) const;
	void			pick();
};
struct character : abilitya, cardpool {
	static character* last;
	locationi*		location;
	void			clear();
	void			encounter();
	void			delayed();
	int				getsuccess() const;
	bool			isallowreroll(ability_s v) const;
	void			leavestreet();
	void			losehalf(ability_s m);
	void			losehalf(cardtype_s m);
	void			movement(locationi* pv);
	int				roll(ability_s v, int m);
};
struct gamei : public character {
};
extern gamei		game;
extern answers		an;