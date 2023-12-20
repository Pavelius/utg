#include "item.h"
#include "roll.h"

static_assert(sizeof(item) == 4, "Struct `item` must be 4 bytes");

item* last_item;

void itema::select(const slice<item>& source) {
	for(auto& e : source) {
		if(e)
			add(&e);
	}
}

void itema::match(wear_s wear, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if((p->geti().wear == wear) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - begin();
}

void itema::match(fntest proc, bool keep) {
	auto ps = begin();
	for(auto p : *this) {
		if(((p->*proc)()) != keep)
			continue;
		*ps++ = p;
	}
	count = ps - begin();
}

item::item(const char* id) {
	item(bsdata<itemi>::find(id));
}

item::item(const itemi* pi) {
	clear();
	if(pi) {
		type = pi - bsdata<itemi>::elements;
		if(pi->count)
			setcount(pi->count);
	}
}

bool item::damage() {
	if(d100() < 50)
		return false;
	if(getcount() > 1)
		setcount(getcount() - 1);
	else if(!broken)
		broken = 1;
	return true;
}

int item::getcount() const {
	if(iscountable())
		return count + 1;
	return 1;
}

void item::setcount(int v) {
	if(v <= 0)
		clear();
	else if(iscountable())
		count = v - 1;
}

void item::add(item& v) {
	if(type != v.type || flags != v.flags)
		return;
	if(!iscountable())
		return;
	unsigned n1 = count + v.count + 1;
	if(n1 >= 0xFFFF) {
		count = 0xFFFF;
		v.count = n1 - count - 1;
	} else {
		count = n1;
		v.clear();
	}
}

bool item::canequip(wear_s v) const {
	auto w = geti().wear;
	switch(w) {
	case FingerRight: case FingerLeft:
		return v == FingerLeft || v == FingerRight;
	default:
		return v == w;
	}
}

void item::addname(stringbuilder& sb) const {
	auto count = getcount();
	auto name = getnm(geti().id);
	if(broken)
		sb.adds(getnm("Broken"));
	if(identified && identified_magic) {
		if(iscursed())
			sb.adds(getnm("Cursed"));
	} else if(identified_magic) {
		if(iscursed() || ismagic())
			sb.adds(getnm("Magical"));
	}
	sb.adds(name);
	if(identified) {
		// TODO: powern name, like 'of defence'
	}
	if(count > 1)
		sb.adds("%1i%Pieces", count);
}

interval item::getdamage() const {
	return geti().damage;
}

int	item::getweight() const {
	auto& ei = geti();
	return getcount() * ei.weight / (ei.count ? ei.count : 1);
}

int item::getcost() const {
	auto& ei = geti();
	return getcount() * ei.cost / (ei.count ? ei.count : 1);
}

const char*	item::getnamef() const {
	static char temp[260];
	stringbuilder sb(temp);
	addname(sb);
	return temp;
}