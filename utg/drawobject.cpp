#include "crt.h"
#include "draw.h"
#include "drawobject.h"

using namespace draw;

void object::paint() const {
	if(resource)
		image(caret.x, caret.y, resource, frame, flags);
}

void draw::paintobjects() {
	for(auto& e : bsdata<object>())
		e.paint();
}

BSDATAC(object, 128)