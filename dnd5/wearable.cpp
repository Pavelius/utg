#include "wearable.h"

int	wearable::getitemcount(unsigned short type) const {
	auto result = 0;
	for(auto& e : wears) {
		if(e.type != type)
			continue;
		result += e.getcount();
	}
	return result;
}

void wearable::addcoins(unsigned v) {
	if(!v)
		return;
	money += v;
}

void wearable::additem(item& v) {
	//if(v.is(Coins)) {
	//	addcoins(v.getcostall());
	//	v.clear();
	//	return;
	//}
	// Try stack existing item
	for(auto& e : backpack()) {
		if(!v)
			break;
		if(e)
			e.add(v);
	}
	if(v) {
		for(auto& e : backpack()) {
			if(!e) {
				e = v;
				v.clear();
				break;
			}
		}
	}
}

void wearable::equip(item& v) {
	for(auto i = MeleeWeapon; i <= Elbows; i = (wear_s)(i + 1)) {
		if(wears[i])
			continue;
		if(!v.is(i))
			continue;
		wears[i] = v;
		v.clear();
		break;
	}
	if(v)
		additem(v);
}

const item*	wearable::getwear(const void* data) const {
	if(data >= wears && data < wears + sizeof(wears) / sizeof(wears[0]))
		return (item*)data;
	return 0;
}

wear_s wearable::getwearslot(const item* data) const {
	if(!data)
		return Backpack;
	return (wear_s)(data - wears);
}

const char* wearable::getwearname(wear_s id) const {
	auto it = wears[id];
	if(it)
		return it.getfullname();
	if(id == MeleeWeapon)
		return getnm("Fist");
	return 0;
}

bool wearable::useitem(const itemi* pi) {
	for(auto& v : backpack()) {
		if(!v.is(pi))
			continue;
		v.use();
		return true;
	}
	return false;
}