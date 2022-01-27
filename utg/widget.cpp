#include "utg.h"
#include "widget.h"

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
	if(p)
		p->proc();
}