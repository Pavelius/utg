#include "item.h"

#pragma once

struct wearable {
	item		wears[Elbows + 1];
	void		additem(item& v);
	void		equip(item& v);
	const char*	getwearname(wear_s id) const;
	bool		isitem(const void* pv) const;
};