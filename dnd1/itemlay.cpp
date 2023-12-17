#include "scenery.h"
#include "itemlay.h"

static itemlay* new_itemlay() {
	for(auto& e : bsdata<itemlay>()) {
		if(!e)
			return &e;
	}
	return bsdata<itemlay>::add();
}

void item::drop() {
	auto p = new_itemlay();
	*((item*)p) = *this;
	p->parent = scene;
	clear();
}

