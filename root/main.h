#include "ability.h"
#include "gender.h"
#include "menu.h"
#include "tag.h"
#include "utg.h"

#pragma once

enum ability_s : unsigned char {
	Charm, Cunning, Finess, Luck, Might
};
enum harm_s : unsigned char {
	Exhaustion, Injury, Depletion, Wear, Morale
};
enum roguish_s : unsigned char {
	Acrobatics, Blindside, Counterfeit, DisableDevice, Hide,
	PickLock, PickPocket, SleightOfHand, Sneak
};
enum skill_s : unsigned char {
	Cleave, ConfuseSenses, Disarm, HarryAGroup, ImproviseWeapon,
	Parry, QuickShot, StormAGroup, TrickShot, ViciousStrike
};
enum tag_s : unsigned char {
	Blunted, Cumbersome, Flexible, Large, Makeshift, MetalBoss,
	Precise, Quick, Reach, Reinforced, Sharp, SteelOfRabbitfolk, SteelOfFoxfolk,
	TightlyWoven, Throwable, Unassuming, Weighty,
	Intimate, Close, Far,
};
enum variant_s : unsigned char {
	NoVariant,
	Ability, Item, Menu, Skill, Tag, Widget,
};
enum slot_s : unsigned char {
	Weapon, Armor, Shield
};
typedef flagable<4> flaga;
struct skilli {
	const char*		id;
};
struct nameable {
	const char*		id;
	void			act(stringbuilder& sb, const char* format, ...) { actv(sb, format, xva_start(format)); }
	void			actv(stringbuilder& sb, const char* format, const char* param);
};
struct harmable {
	char			harm[Morale + 1];
	int				getharm(harm_s v) const { return harm[v]; }
	void			setharm(harm_s v, int i) { harm[v] = i; }
};
struct tagable {
	flaga			tags;
	flaga			skills;
	flaga			feats;
	void			add(tag_s v) { tags.set(v); }
	void			add(skill_s v) { skills.set(v); }
	void			add(roguish_s v) { feats.set(v); }
	bool			is(tag_s v) const { return tags.is(v); }
	bool			is(skill_s v) const { return skills.is(v); }
	bool			is(roguish_s v) const { return feats.is(v); }
};
struct itemi : nameable, tagable {
	char			wear;
};
struct item {
	unsigned char	type;
	unsigned char	wear : 3;
	constexpr explicit operator bool() const { return type != 0; }
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
};
struct equiped {
	item			wears[Shield + 1];
};
struct creature : nameable, equiped {
};