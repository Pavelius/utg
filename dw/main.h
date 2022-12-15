#include "ability.h"
#include "advancement.h"
#include "avatarable.h"
#include "charname.h"
#include "crt.h"
#include "dice.h"
#include "flagable.h"
#include "front.h"
#include "gender.h"
#include "namenpc.h"
#include "collection.h"
#include "result.h"
#include "tag.h"
#include "utg.h"
#include "wearable.h"

#pragma once

template<typename T> struct bsmeta;

struct alignmenti {
	const char*		id;
};
struct classi {
	const char*		id;
	ability_s		abilities[6];
	int				damage;
};
struct dietyi {
	const char*		id;
};
struct statable : moveable {
	char			abilities[Charisma+1];
	void			copy(statable& v) { *this = v; }
	void			apply_ability(int v);
	void			update_player();
};
class creature : public namenpc, public avatarable, public statable, public wearable {
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
	friend bsmeta<creature>;
public:
	explicit operator bool() const { return isvalidname(); }
	void			act(const char* format) const { return actv(*answers::console, format, xva_start(format)); }
	void			generate();
	int				get(ability_s v) const { return abilities[v]; }
	static const char* getavatarst(const void* object);
	int				getbonus(ability_s v) const { return abilities[v] / 2 - 5; }
	dice			getdamage() const;
	const classi&	geti() const { return bsdata<classi>::elements[type]; }
	void			getinfo(stringbuilder& sb) const;
	int				getmaximumhp() const;
	static void		getpropertyst(const void* object, variant v, stringbuilder& sb);
	bool			ismatch(variant v) const;
};
extern creature* player;
inline int			d100() { return rand() % 100; }