#include "bonus.h"
#include "gender.h"
#include "harmable.h"
#include "message.h"
#include "result.h"
#include "tag.h"
#include "utg.h"

#pragma once

enum condition_s : unsigned char {
	GoodState, MiddleState, BadState
};
enum prefix_s : unsigned char {
	Plus, Minus,
};
enum variant_s : unsigned char {
	NoVariant,
	Prefix, Result, Squad,
};
enum tag_s : unsigned char {
	Intimate, Close, Far,
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
struct squad : harmable {
	unsigned char	type;
	tag_s			distance = Far;
	constexpr explicit operator bool() const { return type != 0xFF; }
	void			actv(stringbuilder& sb, const char* format, const char* format_param);
	void			acts(stringbuilder& sb, const char* format, ...) { actv(sb, format, xva_start(format)); }
	void			clear();
	const squadi&	geti() const { return bsdata<squadi>::elements[type]; }
	void			getinfo(stringbuilder& sb) const;
	gender_s		getgender() const { return NoGender; }
	const char*		getname() const { return getnm(geti().id); }
};
struct missioni {
	squad			allies[8];
	squad			enemies[8];
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
extern missioni		game;
VKIND(result_s, Result)