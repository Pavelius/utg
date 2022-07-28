#include "crt.h"
#include "draw_object.h"
#include "posable.h"

using namespace draw;

void posable::setposition(point v) {
	auto p = findobject(this);
	if(!p) {
		p = addobject(v.x, v.y);
		p->data = this;
		p->string = getname();
		if(p->aftercreate)
			p->aftercreate(p);
	}
}

point posable::getposition() const {
	for(auto& e : bsdata<object>()) {
		if(e.data == this)
			return e;
	}
	return {-1000, -1000};
}