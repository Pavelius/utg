#include "item.h"

#pragma once

struct wearable {
	item			wears[Hands + 1];
	int				money;
	void			addcoins(unsigned v);
	void			additem(item& v);
	slice<item>		backpack() { return slice<item>(wears + Backpack, wears + BackpackLast + 1); }
	void			equip(item& v);
	slice<item>		equipment() { return slice<item>(wears + Head, wears + Hands + 1); }
	bool			isitem(unsigned short type, int count = 1) const { return getitemcount(type) >= count; }
	bool			iswear(const void* p) const { return p >= wears && p <= wears + Hands; }
	int				getitemcount(unsigned short type) const;
	int				getmoney() const { return money; }
	item*			getwear(wear_s id) { return wears + id; }
	const char*		getwearname(wear_s id) const;
	wear_s			getwearslot(const item* data) const;
	const item*		getwear(const void* data) const;
	bool			useitem(const itemi* pi);
};
