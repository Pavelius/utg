#include "item.h"
#include "wear.h"

#pragma once

struct wearable {
	item			wears[LastBackpack + 1];
	int				coins;
	void			additem(item& it);
	void			additem(const item& it, int count);
	slice<item>		backpack() { return slice<item>(wears + Backpack, wears + LastBackpack + 1); }
	int				getcoins() const { return coins; }
	int				getuses(int item_type) const;
	bool			removeitem(int item_type, int count);
	void			setcoins(int v) { coins = v; }
};