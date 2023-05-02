#include "collection.h"
#include "dice.h"
#include "feat.h"
#include "list.h"
#include "variant.h"
#include "wear.h"

#pragma once

struct itempoweri : nameable {
	char		magic;
	dice		damage; // Additional damage
	variant		condition; // For weapon when hit - race or alignment for additional damage
	variants	wearing; // Additional wearing effect
};
struct itemi : nameable {
	const char*	unidentified;
	int			cost, damage, count;
	wear_s		wear;
	featable	flags;
	variants	use, wearing;
	itempoweri*	powers[16];
	bool		is(feat_s v) const { return flags.is(v); }
};
class item {
	unsigned short type;
	union {
		unsigned char flags;
		struct {
			unsigned char identified : 1;
			unsigned char personal : 1;
			unsigned char cursed : 1;
		};
	};
	union {
		unsigned char count;
		struct {
			unsigned char power : 4;
			unsigned char broken : 4;
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
	int			getcost() const;
	int			getcount() const;
	dice		getdamage() const;
	const char*	getname() const { return geti().getname(); }
	const itempoweri* getpower() const;
	void		getstatus(stringbuilder& sb) const;
	int			getweight() const;
	bool		iscursed() const { return cursed != 0; }
	bool		iscountable() const { return geti().powers != 0; }
	void		setcount(int v);
};
struct itema : collection<item> {
	void		match(wear_s v, bool keep);
	void		select(const slice<item>& source);
};