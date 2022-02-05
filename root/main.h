#include "ability.h"
#include "bonus.h"
#include "chooselist.h"
#include "gender.h"
#include "message.h"
#include "menu.h"
#include "quest.h"
#include "result.h"
#include "tag.h"
#include "utg.h"

#pragma once

enum ability_s : unsigned char {
	Charm, Cunning, Finess, Luck, Might
};
enum harm_s : unsigned char {
	Injury, Exhaustion, Wear, Depletion,
	Morale, Value,
};
enum special_s : unsigned char {
	Choose, Harm, Range, Ignore,
};
enum tag_s : unsigned char {
	Blunted, Cumbersome, Flexible, Large, Makeshift, MetalBoss,
	Precise, Quick, Reach, Reinforced, Sharp, SteelOfRabbitfolk, SteelOfFoxfolk,
	TightlyWoven, Throwable, Unassuming, Weighty,
	Intimate, Close, Far,
};
enum move_type_s : unsigned char {
	BasicMove, RoguishFeat, WeaponSkill, ReputationMove, TravelMove,
};
enum move_mechanic_s : unsigned char {
	PlainMove,
	Choose1, Choose1or2, Choose1or3, Choose2or3,
	ChooseYouAndEnemy,
};
enum move_s : unsigned char {
	Acrobatics, Blindside, Counterfeit, DisableDevice, Hide,
	PickLock, PickPocket, SleightOfHand, Sneak,
	FigureSomeoneOut, PersuadeNPC,
	ReadTenseSituation, TrickNPC, TrustFate, WreckSomething,
	EngageMelee, GrappleEnemy, TargetSomeone,
	Cleave, ConfuseSenses, Disarm,
	HarryGroup, ImproviseWeapon, Parry,
	QuickShot, StormGroup, TrickShot, ViciousStrike,
	AskFavor, MeetSomeoneImportant, DrawAttention, SwayNPC, MakePointedThread, CommandResources,
};
enum fraction_s : unsigned char {
	Marquisat, ForestAlliance, Denziers,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Bonus, Item, Menu, Move, Resource, Result, Risk, Special, Tag, Widget,
};
typedef flagable<1 + CommandResources / 8> movea;
typedef char choosea[StrongSuccess + 1];
struct nameable {
	const char*		id;
	gender_s		gender = Male;
	void			act(const char* format, ...) const { actv(utg::sb, format, xva_start(format)); }
	void			acts(stringbuilder& sb, const char* format, ...) const { actv(sb, format, xva_start(format)); }
	void			actv(stringbuilder& sb, const char* format, const char* param) const;
	const char*		getname() const { return getnm(id); }
	gender_s		getgender() const { return gender; }
};
struct statable {
	char			abilities[Might + 1];
	int				get(ability_s v) const { return abilities[v]; }
};
struct riski {
	const char*		id;
	int				page;
};
struct movei {
	const char*		id;
	move_type_s		type;
	ability_s		roll;
	move_mechanic_s	mechanic;
};
struct harmi {
	const char*		id;
};
struct speciali {
	const char*		id;
};
struct harmable {
	typedef slice<harm_s> harma;
	char			harm[Value + 1];
	void			add(harm_s v, int i) { harm[v] += i; }
	void			clear();
	void			fix(stringbuilder& sb, harma source) const;
	harm_s			getdefault() const;
	int				getdistinct() const;
	int				getdistinct(harma source) const;
	void			getinfo(stringbuilder& sb, const char* prompt) const;
};
struct tagable {
	flaga			tags;
	movea			moves;
	void			add(tag_s v) { tags.set(v); }
	void			add(move_s v) { moves.set(v); }
	constexpr bool	is(tag_s v) const { return tags.is(v); }
	constexpr bool	is(move_s v) const { return moves.is(v); }
};
struct itemi : nameable, tagable {
	char			wear;
	constexpr bool	isweapon() const { return is(Far) || is(Close) || is(Intimate); }
};
struct item {
	unsigned char	type;
	unsigned char	wear : 3;
	unsigned char	disarmed : 1; // When disarm apply. To end of battle.
	unsigned char	unready : 1; // When parry apply. To few moments, or next turn.
	constexpr explicit operator bool() const { return type != 0; }
	bool			isready() const { return (disarmed + unready) == 0; }
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
};
struct effectable {
	fraction_s		fraction;
	harmable		inflict, suffer, group;
	char			distance, value;
	void			apply(variant v);
	void			clear();
	harmable&		getharm(int m);
};
struct rangeable {
	tag_s			range = Close;
	void			addrange(int v);
	void			setrange(tag_s v) { range = v; }
};
struct creature : nameable, harmable, statable, rangeable {
	void			apply(const effectable& source);
	void			roll(move_s v);
	void			move(move_s v);
};
extern effectable	game;

VKIND(harm_s, Resource)
VKIND(move_s, Move)
VKIND(result_s, Result)
VKIND(special_s, Special)