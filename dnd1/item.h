#include "dice.h"
#include "feat.h"
#include "variant.h"
#include "wear.h"

#pragma once

struct itemi : nameable {
	struct weaponi {
		char	attack;
		dice	damage;
		short 	ammunition;
	};
	const char*	unidentified;
	int			cost, weight, count;
	char		magic, charges;
	weaponi		weapon;
	wear_s		wear;
	featable	flags;
	variants	use, effect;
	bool		is(feat_s v) const { return flags.is(v); }
};
struct item {
	unsigned short type;
	union {
		unsigned short count;
		struct {
			unsigned char identified : 1;
			unsigned char broken : 1;
			unsigned char charges : 5;
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
	const char*	getname() const { return geti().getname(); }
	void		getstatus(stringbuilder& sb) const;
	int			getweight() const;
	bool		iscountable() const { return geti().count != 0; }
	bool		ismagical() const { return geti().magic != 0; }
	void		setcount(int v);
};
struct itema : adat<item*> {
	void		select(const slice<item>& source);
	void		match(wear_s wear, bool keep);
};