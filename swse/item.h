#pragma once
#include "damage.h"
#include "dice.h"
#include "feat.h"
#include "list.h"
#include "size.h"
#include "wear.h"
#include "variant.h"

enum ability_s : unsigned char;

struct itemstat : nameable {
	damage_s		hit;
	dice			damage, stun;
	short			weight, cost;
	char			armor, maxdex, equipment;
	char			size;
	variants		dress;
};
struct itemi : itemstat {
	short			count;
	short			avatar;
	wear_s			wear;
	short			proficiency, focus, specialization;
	variants		use;
	const itemi*	ammunition;
	bool operator==(const itemi& v) const { return this == &v; }
	int				getindex() const { return this - bsdata<itemi>::elements; }
	bool			iscountable() const { return count != 0; }
	bool			ismelee() const { return wear == MeleeWeapon; }
};
class item {
	unsigned short	type;
	unsigned char	count, ammocount;
public:
	explicit operator bool() const { return type != 0; }
	void			add(item& v);
	void			clear() { type = count = ammocount = 0; }
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
	bool			is(const itemi& v) const { return v == geti(); }
	bool			is(const itemi* p) const { return p == &geti(); }
	bool			is(const item& v) const { return type == v.type; }
	bool			iscountable() const { return geti().iscountable(); }
	void			setcount(int v);
	void			use() { setcount(getcount() - 1); }
};
extern item *last_item;
