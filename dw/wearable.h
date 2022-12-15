#include "crt.h"
#include "item.h"
#include "wear.h"

#pragma once

class wearable {
	item			wears[LastBackpack + 1];
	int				coins;
public:
	void			additem(item& it);
	slice<item>		backpack() { return slice<item>(wears + Backpack, wears + LastBackpack + 1); }
	int				getcoins() const { return coins; }
	int				getuses(int item_type) const;
	void			setcoins(int v) { coins = v; }
};