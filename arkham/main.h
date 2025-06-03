#include "ability.h"
#include "answers.h"
#include "draw.h"
#include "flagable.h"
#include "quest.h"
#include "nameable.h"
#include "stringbuilder.h"
#include "realm.h"
#include "tag.h"
#include "variant.h"

#pragma once

typedef short unsigned cardt;

enum gamef_s : unsigned char {
	Bless, Curse, BankLoan, BankLoanNotAllowed, DeputyOfArkham, Retainer, SilverTwilightLodgeMembership,
	ExtraClueDice,
};
enum special_s : unsigned char {
	NoSpecial,
	Tome, PhysicalWeapon, MagicalWeapon, MovementBonus, RefreshAction,
	CombatCheck, EscapeCheck, HorrorCheck, SpellCheck,
};
enum tag_s : unsigned char {
	Ambush, Endless, Undead,
	PhysicalResistance, PhysicalImmunity, MagicalResistance, MagicalImmunity,
	NightmarishI, NightmarishII, OverwhelmingI, OverwhelmingII,
	BonusVsUndead, Exhause, ExhauseToRerollDie, NoSteal, Discard, DiscardOn1, DiscardIfFail, Versatile,
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
	void			add(abilityn v, int i) { abilities[v] += i; }
	void			addabilities(const abilitya& e);
	constexpr int	get(abilityn v) const { return abilities[v]; }
	bool			ispickextra(cardtype_s v) const { return pickextra.is(v); }
	bool			isrestore(abilityn v) const { return restore.is(v); }
	bool			isrerollall(abilityn v) const { return rerollall.is(v); }
	bool			istought(abilityn v) const { return tought.is(v); }
	void			loadability(const abilitya& source);
	void			set(abilityn v, int i) { abilities[v] = i; }
};
struct locationi : nameable {
	cardtype_s		type;
	point			position;
	locationi*		neightboard[8];
	quests			encounters;
	bool			sealed;
	char			clues;
	const quest*	choose(int count = 1) const;
	void			chooseoptions(answers& an) const;
	void			encounter(int count = 1) const;
	int				getclues() const { return clues; }
	location_s		getindex() const { return (location_s)(this - bsdata<locationi>::elements); }
	bool			issealed() const { return sealed; }
	void			phase_encounter();
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
struct realmi : nameable {
};
struct deck : adat<cardt> {
	void			create(cardtype_s type);
	void			drop(cardt v);
	bool			havecard(cardt v) const;
	cardt			pick();
	bool			pick(cardt v);
	void			shuffle();
};
struct cardtypei : nameable {
	deck			cards;
};
struct speciali : nameable {
};
struct cardprotoi : nameable, abilitya {
	cardtype_s		type;
	special_s		special;
	char			hands, difficult, cost, count, pay, bonus;
	variants		effect, fail;
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
	static cardi*	last;
	constexpr explicit operator bool() const { return type != 0; }
	void			apply(variants effect);
	bool			afterroll(abilityn v, int m, special_s special, bool run);
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
	bool			isdoubleclue(abilityn v) const;
	bool			isrerollall(abilityn v) const;
	void			pick(cardtype_s type);
	void			pick(cardtype_s type, int count);
};
struct cardquerry : adat<cardi*, 32> {
	void			add(cardpool& source, cardtype_s type);
	void			add(cardpool& source, special_s type);
	int				get(abilityn v) const;
	bool			isonly(tag_s v) const;
};
struct investigator : nameable, abilitya {
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
	void			additems(answers& an, const char* format, fnallow allow, int param);
	void			apply(variant v);
	bool			cast(cardi& e);
	bool			combat(cardprotoi& source);
	void			create(const char* id);
	void			clear();
	void			damage(abilityn v, int count) { modify(v, -count); }
	void			delayed();
	void			encounter();
	void			equip(cardi* p);
	void			equipment(stringbuilder& sb) const;
	bool			evade(cardprotoi& enemy);
	bool			fight(cardi& source);
	const investigator&	geti() const;
	int				getbonus(abilityn v, int bonus) const;
	int				getcluedices(abilityn v) const;
	int				getcombat() const;
	int				getcount(cardtype_s v) const;
	int				getevade() const;
	int				getfreehands() const;
	int				gethorror() const;
	int				getsuccess() const;
	int				getmaximal(abilityn v) const;
	int				getminimal(abilityn v) const;
	bool			is(gamef_s v) const { return flags.is(v); }
	bool			isallowreroll(abilityn v) const;
	bool			isequiped(const cardi* e) const;
	bool			isready() const { return get(Health) > 0 && get(Sanity) > 0; }
	void			introduction() const;
	void			leavestreet();
	void			loseitems(int count);
	void			modify(abilityn v, int bonus, bool payment = false);
	void			modify(gamef_s v, bool activate = true);
	void			movement(locationi* pv, bool animation = true);
	void			movement(int speed);
	bool			paythrophy(int count, bool run, bool gates, bool monsters);
	void			phase_encounter_arkham();
	void			phase_encounter_other();
	void			phase_movement();
	void			phase_refresh_actions();
	void			phase_refresh_focus();
	void			phase_refresh_update();
	int				roll(abilityn v, int m, special_s special = NoSpecial);
	int				rolld6(int count) const;
	void			showcards();
	void			unequip(cardi* p);
	void			update();
	void			updatecombat();
	void			usehands();
};
struct gamei : public player {
	static quests	quest_other;
	static int		d6();
	static void		apply(const variants& source);
	static void		applyresult(void* result);
	static void		information(const char* format, ...);
	static bool		isallow(const variants& source);
	static void		play();
	static void		turn();
};
extern cardpool		cards;
extern gamei		game;