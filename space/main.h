#include "bonus.h"
#include "gender.h"
#include "message.h"
#include "result.h"
#include "utg.h"

#pragma once

enum condition_s : unsigned char {
	GoodState, MiddleState, BadState
};
enum prefix_s : unsigned char {
	Plus, Minus,
};
enum effect_s : unsigned char {
	InflictHarm, SufferHarm,
	Injury, Gear, Morale, Exhause, Time, Supply,
	Combat, Infiltrate, Driving, Research, Build, Repair, Heal,
};
enum tag_s : unsigned char {
	BadSpot, Opportunity,
	Armored, Explosion, HeavyWeapon, Smashing, Tricky,
	Agile, Tought, Leader,
	Regular, Veteran, Elite,
	Rifle,
	Vehicle,
	Broken, Covered, Hiding, Illuminated,
	Intimate, Close, Far,
};
enum move_s : unsigned char {
	EngageMelee, GrappleEnemy, TargetSomeone,
};
enum movemech_s : unsigned char {
	PlainMove, YouAndEnemy,
};
enum variant_s : unsigned char {
	NoVariant,
	Effect, Move, Prefix, Result, Squad,
};
typedef slice<variant> variantsl;
typedef flagable<8> taga;
struct effecti {
	const char*		id;
};
struct squadi {
	const char*		id;
	taga			tags;
};
struct tagi {
	const char*		id;
};
struct harmable {
	char			injury, gear, morale, exhause;
	taga			tags;
	void			clear();
	int				get(effect_s v) const;
	int				getarmor() const;
	int				getdistinct() const;
	void			getinfo(stringbuilder& sb);
	void			getharm(harmable& result) const;
	int				getmaximum(effect_s v) const;
	constexpr bool	is(tag_s v) const { return tags.is(v); }
	bool			is(condition_s v) const;
	void			set(effect_s v, int i);
};
struct squad : harmable {
	unsigned char	type;
	tag_s			distance = Far;
	constexpr explicit operator bool() const { return type != 0xFF; }
	void			actv(stringbuilder& sb, const char* format, const char* format_param);
	void			acts(stringbuilder& sb, const char* format, ...) { actv(sb, format, xva_start(format)); }
	void			apply(variant v);
	void			clear();
	void			create(int kind);
	const squadi&	geti() const { return bsdata<squadi>::elements[type]; }
	void			getinfo(stringbuilder& sb) const;
	gender_s		getgender() const { return NoGender; }
	const char*		getname() const { return getnm(geti().id); }
	bool			isallow(move_s v) const;
	void			move(move_s action);
};
struct missioni {
	squad			allies[8];
	squad			enemies[8];
	harmable		inflict, suffer;
	unsigned char	enemy, ally;
	char			multiplier, danger, luck, supply, throphy;
	void			add(const char* id, bool enemy = false);
	void			apply(variant v);
	void			apply(variantsl source);
	void			beforemove();
	void			clear();
	bool			dangerous();
	squad&			getally() { return allies[ally]; }
	squad&			getenemy() { return enemies[enemy]; }
	bool			isenemyspoted() const { return enemy != 0xFF; }
	bool			lucky();
};
struct movei {
	const char*		id;
	effect_s		ability;
	movemech_s		mechanic;
	char			choose[3];
	variantsl		fail, success;
};
extern missioni		game;
VKIND(effect_s, Effect)
VKIND(move_s, Move)
VKIND(result_s, Result)