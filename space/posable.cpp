#include "crt.h"
#include "draw_object.h"
#include "posable.h"

using namespace draw;

void posable::setposition(point v) {
	auto p = findobject(this);
	if(!p) {
		p = addobject(v.x, v.y);
		p->data = this;
	}
	position = v;
	if(position != *p) {
		auto po = p->addorder();
		po->x = position.x;
		po->y = position.y;
	}
}