#include "crt.h"
#include "widget.h"

BSDATAC(widget, 128)

fnevent	widget::last_custom;

const widget* widget::find(const char* id) {
	if(!id || id[0] == 0)
		return 0;
	for(auto& e : bsdata<widget>()) {
		if(!e)
			continue;
		if(equal(e.id, id))
			return &e;
	}
	return 0;
}

void widget::paint(const char* id) {
	auto p = find(id);
	if(p) {
		last_custom = p->custom;
		p->proc();
	}
}

void widget::add(const char* id, fnevent proc, fnevent custom) {
	auto p = (widget*)find(id);
	if(!p) {
		p = bsdata<widget>::add();
		p->id = szdup(id);
	}
	if(p)
		p->proc = proc;
}