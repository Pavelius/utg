#include "crt.h"
#include "draw.h"
#include "drawobject.h"

using namespace draw;

object object::def;

void object::initialize() {
	def.fore = colors::text;
	def.x = def.y = -10000;
	def.priority = 64;
	def.size = 32;
	def.alpha = 255;
}

void object::clear() {
	*this = def;
}

void object::setcolorborder() {
	fore = colors::border;
}

void object::setcolorform() {
	fore = colors::form;
}

void object::setcolorwindow() {
	fore = colors::window;
}

void object::paintns() const {
	if(border != figure::None)
		field(border, size);
	if(resource)
		image(caret.x, caret.y, resource, frame, flags);
	if(string)
		textf(string);
}

void object::paint() const {
	auto push_fore = draw::fore;
	auto push_alpha = draw::alpha;
	draw::fore = fore;
	draw::alpha = alpha;
	paintns();
	draw::alpha = push_alpha;
	draw::fore = push_fore;
}

void draw::paintobjects() {
	auto push_clip = clipping;
	setclip({caret.x, caret.y, caret.x + width, caret.y + height});
	for(auto& e : bsdata<object>()) {
		draw::caret = e;
		e.paint();
	}
	clipping = push_clip;
}

void* draw::chooseobject() {
	draw::scene(paintobjects);
	return (void*)getresult();
}

object*	draw::addobject(int x, int y) {
	auto p = bsdata<object>::add();
	*p = object::def;
	p->x = x; p->y = y;
	return p;
}

void draw::clearobjects() {
	bsdata<object>::source.clear();
}

BSDATAC(object, 128)