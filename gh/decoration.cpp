#include "main.h"

static decoration* addnew() {
	for(auto& e : bsdata<decoration>()) {
		if(!e)
			return &e;
	}
	return bsdata<decoration>::add();
}

decoration* decoration::add(const char* id, point hex) {
	auto parent = bsdata<tilei>::find(id);
	if(!parent)
		return 0;
	auto p = addnew();
	p->parent = bsdata<tilei>::find(id);
	p->setposition(hex);
	p->updateui();
	return p;
}