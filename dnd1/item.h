#include "dice.h"
#include "feat.h"
#include "variant.h"
#include "wear.h"

#pragma once

struct enchantmenti {
	variant		special;
	const char*	title;
};
struct enchantmentseti {
	const char*	id;
	sliceu<enchantmenti> elements;
};
struct itemi : nameable {
	struct weaponi {
		char	attack;
		dice	damage;
		short 	ammunition;
	};
	struct armori {
		char	ac, dr;
	};
	int			cost, weight, count;
	armori		armor;
	weaponi		weapon;
	wear_s		wear;
	featable	flags;
	const enchantmentseti* enchantments;
	bool		is(feat_s v) const { return flags.is(v); }
};
struct item {
	unsigned char type, subtype;
	union {
		unsigned short count;
		struct {
			unsigned char identified : 1;
			unsigned char broken : 1;
			unsigned char charge : 5;
			unsigned char count_nocountable;
		};
	};
	explicit operator bool() const { return type != 0; }
	void		add(item& v);
	void		addname(stringbuilder& sb) const;
	bool		canequip(wear_s v) const;
	void		clear() { memset(this, 0, sizeof(*this)); }
	void		create(const char* id, int count = 1);
	void		create(const itemi* pi, int count = 1);
	const itemi& geti() const { return bsdata<itemi>::elements[type]; }
	int			getcost() const;
	int			getcount() const;
	dice		getdamage() const;
	const enchantmenti* getenchant() const;
	const char*	getname() const { return geti().getname(); }
	void		getstatus(stringbuilder& sb) const;
	int			getweight() const;
	bool		iscountable() const { return geti().count != 0; }
	bool		ismagical() const { return getenchant() != 0; }
	void		setcount(int v);
};