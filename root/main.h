#include "ability.h"
#include "bonus.h"
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
	Harm,
};
enum roguish_s : unsigned char {
	Acrobatics, Blindside, Counterfeit, DisableDevice, Hide,
	PickLock, PickPocket, SleightOfHand, Sneak
};
enum tag_s : unsigned char {
	Blunted, Cumbersome, Flexible, Large, Makeshift, MetalBoss,
	Precise, Quick, Reach, Reinforced, Sharp, SteelOfRabbitfolk, SteelOfFoxfolk,
	TightlyWoven, Throwable, Unassuming, Weighty,
	Intimate, Close, Far,
};
enum move_type_s : unsigned char {
	BasicMove, WeaponSkill, ReputationMove, TravelMove,
};
enum move_mechanic_s : unsigned char {
	PlainMove, Choose1, Choose1or2, Choose1or3, Choose2or3,
};
enum move_s : unsigned char {
	AttemptRoguishFeat, FigureSomeoneOut, PersuadeNPC,
	ReadTenseSituation, TrickNPC, TrustFate, WreckSomething,
	EngageMelee, GrappleEnemy, TargetSomeone,
	Cleave, ConfuseSenses, Disarm,
	HarryGroup, ImproviseWeapon, Parry,
	QuickShot, StormGroup, TrickShot, ViciousStrike,
	AskFavor, MeetSomeoneImportant, DrawAttention, SwayNPC, MakePointedThread, CommandResources,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Bonus, Item, Menu, Move, Resource, Result, Tag, Widget,
};
enum slot_s : unsigned char {
	Weapon, Armor, Shield
};
typedef flagable<4> flaga;
typedef char choosea[CriticalSuccess + 1];
struct nameable {
	const char*		id;
	void			act(stringbuilder& sb, const char* format, ...) { actv(sb, format, xva_start(format)); }
	void			actv(stringbuilder& sb, const char* format, const char* param);
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
struct harmable {
	char			harm[Value + 1];
	void			add(harm_s v, int i) { harm[v] += i; }
	harm_s			getdefault() const;
	void			getinfo(stringbuilder& sb, const char* prompt) const;
};
struct tagable {
	flaga			tags;
	flaga			feats;
	flaga			moves;
	void			add(tag_s v) { tags.set(v); }
	void			add(roguish_s v) { feats.set(v); }
	void			add(move_s v) { feats.set(v); }
	bool			is(tag_s v) const { return tags.is(v); }
	bool			is(roguish_s v) const { return feats.is(v); }
};
struct itemi : nameable, tagable {
	char			wear;
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
	harmable		inflict, suffer, group;
	char			distance, value;
	void			apply(variant v);
	void			clear();
};
struct equiped {
	item			wears[Shield + 1];
};
struct creature : nameable, equiped {
};

VKIND(harm_s, Resource)