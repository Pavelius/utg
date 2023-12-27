#pragma once

#include "attackable.h"
#include "collection.h"
#include "interval.h"
#include "feat.h"
#include "list.h"
#include "typeable.h"
#include "variant.h"
#include "wear.h"

enum spell_s : unsigned char;

struct itemi : attacki {
	unsigned	cost, weight, count;
	itemi		*ammunition, *basic;
	wear_s		wear;
	featable	flags;
	listi*		powers;
	variants	use, wearing;
	bool		is(feat_s v) const { return flags.is(v); }
};
class item : public typeable<itemi, unsigned char> {
	union {
		unsigned char flags;
		struct {
			unsigned char identified : 1;
			unsigned char identified_magic : 1;
			unsigned char broken : 2;
			unsigned char power : 4; // 0 - is not magical
		};
	};
	unsigned short count;
public:
	item(const char* id);
	item(const itemi* pi);
	item() = default;
	explicit operator bool() const { return type != 0; }
	void		act(char separator, const char* format, ...) const;
	bool		actid(const char* id, const char* p1, char separator = ' ') const;
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
	const char*	getnamef() const;
	variant		getpower() const;
	void		getstatus(stringbuilder& sb) const;
	int			getweight() const;
	bool		isallowspell() const;
	bool		isbroken() const { return broken >= 3; }
	bool		iscountable() const { return true; }
	bool		iscursed() const { return getpower().counter < 0; }
	bool		isidentified() const { return identified != 0; }
	bool		ismagic() const { return getpower().counter != 0; }
	bool		isnatural() const { return geti().weight==0; }
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