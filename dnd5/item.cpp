#include "crt.h"
#include "item.h"
#include "stringbuilder.h"

item* last_item;

static_assert(sizeof(item) == 4, "Invalid size of `item` structure");

static int d100() {
	return rand() % 100;
}

void item::clear() {
	memset(this, 0, sizeof(*this));
}

const itemi& item::geti() const {
	return bsdata<itemi>::elements[type];
}

const char* item::getname() const {
	return bsdata<itemi>::elements[type].getname();
}

int item::getcost() const {
	return geti().cost;
}

int item::getcostall() const {
	auto& ei = geti();
	auto c = getcount();
	if(ei.count > 1) {
		c = c / ei.count;
		if(c <= 0)
			c = 1;
	}
	return ei.cost * c;
}

void item::setcount(int v) {
	if(v <= 0)
		clear();
	else if(iscountable())
		count = v - 1;
}

int item::getcount() const {
	return type ? (iscountable() ? count + 1 : 1) : 0;
}

void item::add(item& v) {
	if(type != v.type)
		return;
	if(iscountable()) {
		unsigned n1 = count + v.count + 1;
		if(n1 >= 0xFF) {
			count = 0xFF;
			v.count = n1 - count - 1;
		} else {
			count = n1;
			v.clear();
		}
	}
}

void item::create(const itemi* pi, int count) {
	if(!pi)
		return;
	clear();
	type = pi - bsdata<itemi>::elements;
	if(pi->count)
		setcount(count * pi->count);
	else
		setcount(count);
}

bool item::is(wear_s v) const {
	auto wear = geti().wear;
	return (wear == v);
}

const char*	item::getfullname(int price_percent) const {
	static char temp[260];
	stringbuilder sb(temp);
	auto count = getcount();
	auto pn = getname();
	auto vw = stringbuilder::getgender(pn);
	sb.adds("%1", getname());
	if(count > 1)
		sb.adds("%1i %-Pieces", count);
	sb.lower();
	if(price_percent) {
		auto cost = getcost() * price_percent / 100;
		sb.adds("%-Cost %1i %-Coins", cost);
	}
	return temp;
}

int	item::getweight() const {
	auto& ei = geti();
	auto result = ei.weight;
	return result * getcount();
}

bool item::is(const itemi* pv) const {
	if(!pv)
		return false;
	return (bsdata<itemi>::elements + type) == pv;
}

void item::use() {
}