#pragma once

#include "collection.h"
#include "dice.h"
#include "feat.h"
#include "list.h"
#include "typeable.h"
#include "variant.h"
#include "wear.h"

struct itempoweri : nameable {
	char		magic;
	dice		damage; // Additional damage
	variant		condition; // For weapon when hit - race or alignment for additional damage
	variants	wearing; // Additional wearing effect
};
struct itemi : nameable {
	unsigned	cost, weight, count;
	itemi*		basic;
	dice		damage;
	itemi*		ammunition;
	wear_s		wear;
	featable	flags;
	variants	use, wearing;
	itempoweri*	powers[16];
	bool		is(feat_s v) const { return flags.is(v); }
};
class item : public typeable<itemi> {
	union {
		unsigned char flags;
		struct {
			unsigned char identified : 1;
			unsigned char cursed : 1;
			unsigned char personal : 1;
		};
	};
	union {
		unsigned char count;
		struct {
			unsigned char power : 4;
			unsigned char broken : 1;
			unsigned char charges : 2;
		};
	};
public:
	item(const char* id);
	item(const itemi* pi);
	item() = default;
	explicit operator bool() const { return type != 0; }
	void		add(item& v);
	void		addname(stringbuilder& sb) const;
	bool		canequip(wear_s v) const;
	void		clear() { memset(this, 0, sizeof(*this)); }
	bool		damage();
	int			getcost() const;
	int			getcount() const;
	dice		getdamage() const;
	const itempoweri* getpower() const;
	void		getstatus(stringbuilder& sb) const;
	int			getweight() const;
	bool		isbroken() const { return !iscountable() && broken != 0; }
	bool		iscursed() const { return cursed != 0; }
	bool		iscountable() const { return geti().powers[0] == 0; }
	bool		isready() const;
	void		setcount(int v);
};
struct itema : collection<item> {
	void		match(wear_s v, bool keep);
	void		select(const slice<item>& source);
};
extern item* last_item;