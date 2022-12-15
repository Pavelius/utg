#include "crt.h"
#include "item.h"
#include "wear.h"

#pragma once

class wearable {
	item			wears[LastBackpack + 1];
public:
	void			additem(item& it);
	slice<item>		backpack() { return slice<item>(wears + Backpack, wears + LastBackpack + 1); }
};