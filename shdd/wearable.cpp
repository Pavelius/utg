#include "wearable.h"

unsigned wearable::coins;

void wearable::addcoins(item& v) {
	if(v.geti().is(Coins)) {
		coins += v.getcost();
		v.clear();
	}
}

void wearable::additem(item& v) {
	addcoins(v);
	if(!v)
		return;
	// Try stack existing item
	for(auto i = Backpack; i <= BackpackLast; i = (wear_s)(i+1)) {
		if(!v)
			break;
		if(wears[i])
			wears[i].add(v);
	}
	if(v) {
		// Try add new item
		for(auto& e : backpack()) {
			if(!e) {
				e = v;
				v.clear();
				last_item = &e;
				break;
			}
		}
	}
}

void wearable::equip(item& v) {
	addcoins(v);
	if(!v)
		return;
	for(auto i = MeleeWeapon; i <= Elbows; i = (wear_s)(i + 1)) {
		if(wears[i])
			continue;
		if(!v.canequip(i))
			continue;
		wears[i] = v;
		v.clear();
		last_item = &wears[i];
		break;
	}
}

void wearable::addequip(item& v) {
	addcoins(v);
	if(v)
		equip(v);
	if(v)
		additem(v);
}

bool wearable::isitem(const void* pv) const {
	return pv >= wears && pv < wears + sizeof(wears) / sizeof(wears[0]);
}

const char* wearable::getwearname(wear_s id) const {
	auto it = wears[id];
	if(it)
		return it.getname();
	if(id == MeleeWeapon)
		return getnm("Fist");
	return 0;
}