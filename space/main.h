#include "bonus.h"
#include "utg.h"
#include "result.h"

#pragma once

enum condition_s : unsigned char {
	GoodState, MiddleState, BadState
};
enum effect_s : unsigned char {
	Injury, Gear, Morale, Exhause, Time,
	Armor,
	InflictHarm, SufferHarm,
};
enum tag_s : unsigned char {
	Armored, Explosion, HeavyWeapon,
	Tought,
	Regular, Veteran, Elite,
	Vehicle,
	Intimate, Close, Far,
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
	unsigned char	combat, infiltrate, research, build, repair, heal, count, cost;
};
struct statable {
	unsigned char	injury : 4;
	unsigned char	gear : 4;
	unsigned char	morale : 4;
	unsigned char	exhause : 4;
	flaga			tags;
	constexpr bool	is(tag_s v) const { return tags.is(v); }
	void			apply(variant v);
	int				get(effect_s v) const;
	int				getmaximum(effect_s v) const;
	void			set(effect_s v, int i);
};
struct squad {
	unsigned char	type;
	const squadi&	geti() const { return bsdata<squadi>::elements[type]; }
	void			create(unsigned char type);
	void			getinfo(stringbuilder& sb) const;
	bool			is(condition_s v) const;
	void			kill();
	void			wound();
};
struct missioni {
	squad			allies[8];
	char			danger, luck, supply;
	char			throphy;
	void			add(const char* id);
	void			add(const squad& v);
	void			clear();
	bool			dangerous();
	void			paint() const;
	bool			lucky();
};
extern missioni		mission;
VKIND(effect_s, Effect)