#include "ability.h"
#include "advancement.h"
#include "avatarable.h"
#include "dice.h"
#include "flagable.h"
#include "front.h"
#include "gender.h"
#include "namenpc.h"
#include "collection.h"
#include "result.h"
#include "statable.h"
#include "draw_utg.h"
#include "wearable.h"

#pragma once

struct classi;

struct creature : public namenpc, public avatarable, public statable, public wearable {
	unsigned char	alignment, type, diety;
	racen			race;
	taga			tags;
	statable		basic;
	explicit operator bool() const { return isvalidname(); }
	void			apply_advance();
	void			choose_avatar();
	void			choose_abilities();
	void			choose_name();
	void			finish();
	void			random_ability();
	void			update();
	void			act(const char* format) const { return actv(*answers::console, format, xva_start(format)); }
	void			generate();
	int				get(abilityn v) const { return abilities[v]; }
	int				getbonus(abilityn v) const { return abilities[v] / 2 - 5; }
	dice			getdamage() const;
	const classi&	geti() const;
	int				getmaximumhp() const;
	static void		getpropertyst(const void* object, variant v, stringbuilder& sb);
	bool			is(moven v) const { return moveable::is(v); }
	bool			is(tagn v) const { return tags.is(v); }
	bool			ismatch(variant v) const;
};
extern creature* player;
extern creature* opponent;

const char* party_avatar(const void* object);

creature* party_maximum(abilityn v, tagn t);
creature* party_maximum(abilityn v);