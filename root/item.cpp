#include "answers.h"
#include "item.h"

const char*	item::getname() const {
	return getnm(geti().id);
}

void item::create(unsigned char item_type) {
	type = item_type;
	auto& ei = geti();
	wear_max = ei.wear;
	wear = wear_max;
}

void item::destory() {
	if(is(Durable)) {
		if(wear_max) {
			wear_max--;
			draw::warning(getnm("ItemDurableCracked"), getname());
			return;
		}
	}
	draw::warning(getnm("ItemDestoryed"), getname());
	clear();
}

void item::damage(int value) {
	if(value <= 0)
		return;
	marked = 1;
	if(!wear)
		destory();
	else if(wear < value)
		wear = 0;
	else
		wear -= value;
}

int item::getload() const {
	auto r = 0;
	if(is(Load))
		r++;
	if(is(Weighty))
		r++;
	if(is(Comfortable))
		r--;
	return imax(r, 0);
}