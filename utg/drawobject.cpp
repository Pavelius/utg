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

static void textcn(const char* string) {
	auto push_caret = caret;
	caret.x -= textw(string) / 2;
	text(string);
	caret = push_caret;
}

void object::paintns() const {
	if(border != figure::None)
		field(border, size);
	if(resource)
		image(caret.x, caret.y, resource, frame, flags);
	if(proc)
		proc();
	if(string)
		textcn(string);
}

void object::paint() const {
	auto push_fore = draw::fore;
	auto push_alpha = draw::alpha;
	auto push_size = draw::fsize;
	draw::fsize = size;
	draw::fore = fore;
	draw::alpha = alpha;
	paintns();
	draw::fsize = push_size;
	draw::alpha = push_alpha;
	draw::fore = push_fore;
}

static size_t getobjects(object** pb, object** pe) {
	auto ps = pb;
	for(auto& e : bsdata<object>()) {
		if(ps < pe)
			*ps++ = &e;
	}
	return ps - pb;
}

static int compare(const void* v1, const void* v2) {
	auto p1 = *((object**)v1);
	auto p2 = *((object**)v2);
	if(p1->priority!=p2->priority)
		return p1->priority - p2->priority;
	if(p1->y != p2->y)
		return p1->y - p2->y;
	return p1->x - p2->x;
}

static void sortobjects(object** pb, size_t count) {
	qsort(pb, count, sizeof(pb[0]), compare);
}

void draw::paintobjects() {
	auto push_clip = clipping;
	setclip({caret.x, caret.y, caret.x + width, caret.y + height});
	object* source[128];
	auto count = getobjects(source, source + sizeof(source) / sizeof(source[0]));
	sortobjects(source, count);
	for(size_t i = 0; i < count; i++) {
		draw::caret = *source[i] + draw::camera;
		source[i]->paint();
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

const sprite* draw::getres(const char* name) {
	return gres(name, "art/objects", {}, -10000, -10000);
}

BSDATAC(object, 128)