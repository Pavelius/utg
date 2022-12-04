#include "damage.h"
#include "dice.h"
#include "feat.h"
#include "list.h"
#include "size.h"
#include "wear.h"
#include "variant.h"

#pragma once

enum ability_s : unsigned char;

struct itemstat : nameable {
	damages			hit;
	dice			damage, stun;
	short			weight, cost;
	char			size;
	featable		feats;
	variants		dress;
};
struct itemvariety : nameable {
	const itemstat*	elements[15];
};
struct itemi : itemstat {
	short			count;
	short			avatar;
	wear_s			wear;
	variants		use;
	const itemi*	ammunition;
	bool operator==(const itemi& v) const { return this == &v; }
	int				getindex() const { return this - bsdata<itemi>::elements; }
	bool			iscountable() const { return count != 0; }
	bool			ismelee() const { return wear == MeleeWeapon || wear == MeleeWeaponOffhand; }
};
class item {
	unsigned short	type;
	unsigned char	count, ammocount;
public:
	explicit operator bool() const { return type != 0; }
	void			add(item& v);
	bool			canequip(wear_s v) const;
	void			clear() { type = count = 0; }
	void			create(const char* id, int count = 1) { create(bsdata<itemi>::find(id), count); }
	void			create(const itemi* pi, int count = 1);
	void			damage() {}
	int				get(unsigned fo) const;
	short unsigned	getkind() const { return type; }
	const itemi&	geti() const { return bsdata<itemi>::elements[type]; }
	void			getinfo(stringbuilder& sb) const;
	int				getcost() const;
	int				getcostall() const;
	int				getcount() const;
	const char*		getname() const { return geti().getname(); }
	static const char* getname(const void* p) { return ((item*)p)->getfullname(); }
	const char*		getfullname(int price_percent = 0) const;
	int				getweight() const;
	bool			is(wear_s v) const;
	bool			is(feat_s v) const { return geti().feats.is(v); }
	bool			is(const itemi& v) const { return v == geti(); }
	bool			is(const itemi* p) const { return p == &geti(); }
	bool			is(const item& v) const { return type == v.type; }
	bool			iscountable() const { return geti().iscountable(); }
	void			setcount(int v);
	void			setupgrade(const itemstat* pv);
	void			upgrade(int chance_suffix, int chance_prefix, int level);
	void			use() { setcount(getcount() - 1); }
};
extern item *last_item;
