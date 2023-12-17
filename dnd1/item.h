#pragma once

#include "collection.h"
#include "dice.h"
#include "feat.h"
#include "list.h"
#include "typeable.h"
#include "variant.h"
#include "wear.h"

struct itemi : nameable {
	unsigned	cost, weight, count;
	itemi*		basic;
	dice		damage;
	itemi*		ammunition;
	wear_s		wear;
	featable	flags;
	variants	use, wearing, powers;
	bool		is(feat_s v) const { return flags.is(v); }
};
class item : public typeable<itemi> {
	union {
		unsigned char flags;
		struct {
			unsigned char identified : 1;
			unsigned char cursed : 1;
			unsigned char broken : 1;
		};
	};
	unsigned char count;
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
	void		drop();
	int			getcost() const;
	int			getcount() const;
	dice		getdamage() const;
	void		getstatus(stringbuilder& sb) const;
	int			getweight() const;
	bool		isbroken() const { return broken != 0; }
	bool		iscountable() const { return true; }
	bool		iscursed() const { return cursed != 0; }
	bool		isidentified() const { return identified != 0; }
	bool		ismagic() const { return false; }
	bool		isready() const;
	void		setcount(int v);
};
struct itema : collection<item> {
	void		match(wear_s v, bool keep);
	void		select(const slice<item>& source);
};
extern item* last_item;