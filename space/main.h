#include "bonus.h"
#include "utg.h"
#include "result.h"

#pragma once

enum condition_s : unsigned char {
	GoodState, MiddleState, BadState
};
enum effect_s : unsigned char {
	InflictHarm, SufferHarm,
	Injury, Gear, Morale, Exhause, Time,
	Armor, Harm,
	Combat, Infiltrate, Driving, Research, Build, Repair, Heal,
};
enum tag_s : unsigned char {
	Explosion, HeavyWeapon,
	Tought,
	Regular, Veteran, Elite,
	Vehicle,
	BadSpot, Opportunity,
	Hiding, Covered, Illuminated,
	Intimate, Close, Far,
};
enum move_s : unsigned char {
	EngageMelee, GrappleEnemy, TargetSomeone,
};
enum variant_s : unsigned char {
	NoVariant,
	Effect, Squad,
};
struct effecti {
	const char*	id;
};
struct squadi {
	const char*		id;
	unsigned char	injury, gear, exhause, morale;
};
struct tagi {
	const char*		id;
	effect_s		effects[4];
	bool			is(effect_s v) const { for(auto e : effects) if(e == v) return true; return false; }
};
struct squad {
	unsigned char	type;
	unsigned char	injury : 4;
	unsigned char	gear : 4;
	unsigned char	morale : 4;
	unsigned char	exhause : 4;
	unsigned char	armor : 4;
	tag_s			distance = Far;
	flaga			tags;
	void			apply(variant v);
	int				get(effect_s v) const;
	const squadi&	geti() const { return bsdata<squadi>::elements[type]; }
	void			getinfo(stringbuilder& sb) const;
	int				getmaximum(effect_s v) const;
	bool			is(condition_s v) const;
	constexpr bool	is(tag_s v) const { return tags.is(v); }
	bool			isallow(move_s v) const;
	void			set(effect_s v, int i);
};
struct missioni {
	adat<squad, 8>	allies;
	char			danger, luck, supply, throphy;
	void			add(const char* id);
	void			add(const squad& v);
	void			clear();
	bool			dangerous();
	void			paint() const;
	bool			lucky();
};
struct movei {
	const char*		id;
};
extern missioni		mission;
VKIND(effect_s, Effect)