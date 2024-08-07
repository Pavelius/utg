#include "collection.h"
#include "dice.h"
#include "feat.h"
#include "list.h"
#include "variant.h"
#include "wear.h"

#pragma once

struct itemi : nameable {
	const char*	unidentified;
	int			cost, damage, count, slot, attack;
	wear_s		wear;
	featable	flags;
	variants	use, wearing;
	listi*		powers;
	bool		is(feat_s v) const { return flags.is(v); }
};
class item {
	unsigned short type;
	unsigned char count; // count or superpower
	union {
		unsigned char flags;
		struct {
			unsigned char identified : 1;
			unsigned char damaged : 2;
			unsigned char cursed : 1;
			unsigned char masterwork : 1;
			unsigned char charges : 3;
		};
	};
public:
	explicit operator bool() const { return type != 0; }
	void		add(item& v);
	void		addname(stringbuilder& sb) const;
	bool		canequip(wear_s v) const;
	void		clear() { memset(this, 0, sizeof(*this)); }
	void		create(unsigned short type, int count);
	void		create(const char* id, int count);
	void		create(const itemi* pi, int count);
	void		damage();
	const itemi& geti() const { return bsdata<itemi>::elements[type]; }
	int			getattacks() const;
	int			getcost() const;
	int			getcount() const;
	dice		getdamage() const;
	const char*	getname() const { return geti().getname(); }
	variant		getpower() const;
	void		getstatus(stringbuilder& sb) const;
	int			getweight() const;
	bool		is(feat_s v) const { return geti().is(v); }
	bool		iscursed() const { return cursed != 0; }
	bool		iscountable() const { return geti().powers == 0; }
	void		setcount(int v);
	void		setcursed(int v) { cursed = (v >= 0) ? 1 : 0; }
	void		setidentify(int v) { identified = v; }
	void		setmasterwork(int v) { masterwork = (v >= 0) ? 1 : 0; }
};
struct itema : collection<item> {
	void		match(wear_s v, bool keep);
	void		select(const slice<item>& source);
};
struct itemad : adat<item> {
};
extern itemad items;
extern item* last_item;