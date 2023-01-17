#include "item.h"

#pragma once

struct wearable {
	item		wears[Elbows + 1];
	void		additem(item& v);
	slice<item>	allitems() { return slice<item>(wears, wears + Elbows + 1); }
	slice<item>	equipment() { return slice<item>(wears + MeleeWeapon, wears + Elbows + 1); }
	void		equip(item& v);
	const char*	getwearname(wear_s id) const;
	bool		isitem(const void* pv) const;
};