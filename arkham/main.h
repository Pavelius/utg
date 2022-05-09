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
enum gamef_s : unsigned char {
	Bless, Curse, BankLoan, BankLoanNotAllowed, Retainer, SilverTwilightLodgeMembership,
	ExtraClueDice,
};
enum special_s : unsigned char {
	NoSpecial,
	Tome, PhysicalWeapon, MagicalWeapon, MovementBonus,
	CombatCheck, EscapeCheck, HorrorCheck, SpellCheck,
};
enum tag_s : unsigned char {
	Ambush, Endless, Undead,
	PhysicalResistance, PhysicalImmunity, MagicalResistance, MagicalImmunity,
	NightmarishI, NightmarishII, OverwhelmingI, OverwhelmingII,
	BonusVsUndead, Exhause, ExhauseToRerollDie, NoSteal, Discard, Versatile,
};
enum location_s : unsigned char {
	PlayerArea,
	LostInTimeAndSpaceArea, Outskirts, Sky, Prison,
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
struct gamefi {
	const char*		id;
};
struct abilitya {
	char			abilities[Focus + 1];
	abilityf		rerollall, tought, restore;
	cardf			pickextra, scavenge;
	void			add(ability_s v, int i) { abilities[v] += i; }
	void			addabilities(const abilitya& e);
	constexpr int	get(ability_s v) const { return abilities[v]; }
	bool			ispickextra(cardtype_s v) const { return pickextra.is(v); }
	bool			isrestore(ability_s v) const { return restore.is(v); }
	bool			isrerollall(ability_s v) const { return rerollall.is(v); }
	bool			istought(ability_s v) const { return tought.is(v); }
	void			loadability(const abilitya& source);
	void			set(ability_s v, int i) { abilities[v] = i; }
};
struct nameablei {
	const char*		id;
};
struct locationi : nameablei {
	cardtype_s		type;
	point			position;
	locationi*		neightboard[8];
	quests			encounters;
	const quest*	choose(int count = 1) const;
	void			encounter(int count = 1) const;
	location_s		getindex() const { return (location_s)(this - bsdata<locationi>::elements); }
};
struct scripti {
	typedef void (*fnevent)(int counter, int param);
	typedef void (*fninfo)(stringbuilder& sb, const scripti& e, int counter);
	typedef bool (*fntest)(int counter, int param);
	const char*		id;
	fnevent			proc;
	int				param;
	fntest			choose;
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
struct speciali {
	const char*		id;
};
struct cardprotoi : nameablei, abilitya {
	cardtype_s		type;
	special_s		special;
	char			hands, difficult, cost, count, pay, bonus;
	variants		effect;
	realma			realms;
	taga			tags;
	int				getcombat(const cardprotoi& enemy) const;
	int				getcost(int discount) const;
	bool			is(tag_s v) const { return tags.is(v); }
};
struct cardi {
	short unsigned	type;
	unsigned char	exhaused : 1;
	unsigned char	uses : 3;
	location_s		area;
	constexpr explicit operator bool() const { return type != 0; }
	bool			afterroll(ability_s v, int m, special_s special, bool run);
	void			clear();
	void			discard();
	cardprotoi&		geti() const { return bsdata<cardprotoi>::elements[type]; }
	locationi*		getlocation() const { return bsdata<locationi>::elements + area; }
	bool			is(location_s a) const { return area == a; }
	bool			isactive() const { return !exhaused; }
	void			use();
};
struct cardpool : public adat<cardi> {
	void			add(cardt v, location_s a = PlayerArea);
	void			discard();
	bool			have(cardt v) const;
	bool			isdoubleclue(ability_s v) const;
	bool			isrerollall(ability_s v) const;
	void			pick(cardtype_s type);
	void			pick(cardtype_s type, int count);
};
struct cardquerry : adat<cardi*, 32> {
	void			add(cardpool& source, cardtype_s type);
	int				get(ability_s v) const;
};
struct investigator : nameablei, abilitya {
	variants		extra;
	locationi*		location;
	explicit operator bool() const { return location != 0; }
};
struct player : abilitya {
	static player*	last;
	abilitya		original;
	locationi*		location;
	flagable<2>		flags;
	unsigned char	investigator_index;
	char			m_health, m_sanity;
	char			focus[3];
	cardi*			hands[4];
	void			apply(variant v);
	bool			combat(cardprotoi& source);
	void			create(const char* id);
	void			clear();
	void			damage(ability_s v, int count) { modify(v, -count); }
	void			delayed();
	void			encounter();
	void			equip(cardi* p);
	void			equipment(stringbuilder& sb) const;
	bool			evade(cardprotoi& enemy);
	bool			fight(cardi& source);
	const investigator&	geti() const;
	int				getbonus(ability_s v, int bonus) const;
	int				getcluedices(ability_s v) const;
	int				getcombat() const;
	int				getevade() const;
	int				getfreehands() const;
	int				gethorror() const;
	int				getsuccess() const;
	int				getmaximal(ability_s v) const;
	int				getminimal(ability_s v) const;
	bool			is(gamef_s v) const { return flags.is(v); }
	bool			isallowreroll(ability_s v) const;
	bool			isequiped(const cardi* e) const;
	bool			isready() const { return get(Health) > 0 && get(Sanity) > 0; }
	void			introduction() const;
	void			leavestreet();
	void			losehalf(cardtype_s m);
	void			modify(ability_s v, int bonus);
	void			movement(locationi* pv, bool animation = true);
	void			movement(int speed);
	bool			paythrophy(int count, bool run, bool gates, bool monsters);
	void			refocus();
	void			refreshcards();
	int				roll(ability_s v, int m, special_s special = NoSpecial);
	int				rolld6(int count) const;
	void			showcards();
	void			setflag(gamef_s v, bool activate = true);
	void			unequip(cardi* p);
	void			update();
	void			updatecombat();
	void			usehands();
};
struct gamei : public player {
	static quests	quest_other;
	static int		d6();
	static void		information(const char* format, ...);
};
extern cardpool			cards;
extern gamei			game;
extern answers			an;