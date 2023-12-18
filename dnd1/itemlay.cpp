#include "scenery.h"
#include "itemlay.h"

static itemlay* new_itemlay() {
	for(auto& e : bsdata<itemlay>()) {
		if(!e)
			return &e;
	}
	return bsdata<itemlay>::add();
}

static void add_existing(variant parent, item& it) {
	if(!it.iscountable())
		return;
	for(auto& e : bsdata<itemlay>()) {
		if(e && e.parent == parent) {
			e.add(it);
			if(!it)
				break;
		}
	}
}

static void add_newstock(variant parent, item& it) {
	auto p = new_itemlay();
	*((item*)p) = it;
	p->parent = scene;
	it.clear();
}

void add_container(variant parent, item& it) {
	add_existing(parent, it);
	add_newstock(parent, it);
}

void itema::select(variant container) {
	auto ps = begin();
	for(auto& e : bsdata<itemlay>()) {
		if(e && e.parent == container)
			*ps++ = &e;
	}
	count = ps - begin();
}

void item::drop() {
	add_container(scene, *this);
}

