#pragma once

#include "collection.h"
#include "interval.h"
#include "feat.h"
#include "list.h"
#include "typeable.h"
#include "variant.h"
#include "wear.h"

enum spell_s : unsigned char;

struct itemi : nameable {
	unsigned	cost, weight, count;
	interval	damage;
	itemi		*ammunition, *basic;
	wear_s		wear;
	featable	flags;
	variants	use, wearing, powers;
	bool		is(feat_s v) const { return flags.is(v); }
};
class item : public typeable<itemi, unsigned char> {
	union {
		unsigned char flags;
		struct {
			unsigned char identified : 1;
			unsigned char cursed : 1;
			unsigned char broken : 1;
		};
	};
	unsigned short count;
public:
	item(const char* id);
	item(const itemi* pi);
	item() = default;
	explicit operator bool() const { return type != 0; }
	void		act(char separator, const char* format, ...) const;
	void		add(item& v);
	void		addname(stringbuilder& sb) const;
	bool		apply(spell_s id, int level, bool run);
	bool		canequip(wear_s v) const;
	void		clear() { memset(this, 0, sizeof(*this)); }
	bool		damage();
	void		drop();
	int			getcost() const;
	int			getcount() const;
	interval	getdamage() const;
	void		getstatus(stringbuilder& sb) const;
	int			getweight() const;
	bool		isallowspell() const;
	bool		isbroken() const { return broken != 0; }
	bool		iscountable() const { return true; }
	bool		iscursed() const { return cursed != 0; }
	bool		isidentified() const { return identified != 0; }
	bool		ismagic() const { return false; }
	bool		isready() const;
	void		setcount(int v);
};
struct itema : collection<item> {
	typedef bool(item::*fntest)() const;
	void		match(wear_s v, bool keep);
	void		match(fntest v, bool keep);
	void		select(const slice<item>& source);
	void		select(variant container);
};
extern itema items;
extern item* last_item;