#include "ability.h"
#include "advancement.h"
#include "avatarable.h"
#include "crt.h"
#include "dice.h"
#include "flagable.h"
#include "front.h"
#include "gender.h"
#include "namenpc.h"
#include "collection.h"
#include "result.h"
#include "statable.h"
#include "stringlist.h"
//#include "tag.h"
#include "draw_utg.h"
#include "wearable.h"

#pragma once

struct classi;

struct creature : public namenpc, public avatarable, public statable, public wearable {
	unsigned char	alignment, type, diety;
	race_s			race;
	statable		basic;
	void			apply_advance();
	void			choose_avatar();
	void			choose_abilities();
	void			choose_name();
	void			finish();
	void			random_ability();
	void			update();
	explicit operator bool() const { return isvalidname(); }
	void			act(const char* format) const { return actv(*answers::console, format, xva_start(format)); }
	void			generate();
	int				get(ability_s v) const { return abilities[v]; }
	static const char* getavatarst(const void* object);
	int				getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
	dice			getdamage() const;
	const classi&	geti() const;
	int				getmaximumhp() const;
	static void		getpropertyst(const void* object, variant v, stringbuilder& sb);
	bool			ismatch(variant v) const;
};
extern creature* player;