#include "draw.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

const int size = 256;

void status_info(void) {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x += 4;
	caret.y += 4;
	fore = push_fore;
	caret = push_caret;
	caret.y += 4 * 4 + 24;
}

static void add_system(systemi* ps, point pt) {
	auto p = addobject(pt.x, pt.y);
	p->priority = 1;
	p->data = ps;
	p->shape = figure::Hexagon;
	p->size = size;
}

static void add_planet(planeti* ps, point pt) {
	auto p = addobject(pt.x, pt.y);
	p->priority = 5;
	p->data = ps;
	p->frame = 1;
	p->resource = gres("planets", "art/objects");
	p->size = 56;
	p->font = metrics::h2;
	p->string = ps->getname();
}

void systemi::paint() const {
}

static void textvalue(figure shape, const char* format) {
	auto push_caret = caret;
	field(shape, 12);
	caret.x -= (textw(format) + 1) / 2;
	caret.y -= (texth() + 1) / 2;
	text(format);
	caret = push_caret;
}

static void textvalue(figure shape, int v) {
	if(!v)
		return;
	char temp[260]; stringbuilder sb(temp);
	sb.add("%1i", v);
	textvalue(shape, temp);
}

void planeti::paint() const {
	auto push_caret = caret;
	auto push_fore = fore;
	caret.x -= 58; caret.y += 42;
	fore = colors::blue.mix(colors::white);
	textvalue(figure::Circle, get(Influence));
	caret = push_caret;
	caret.x -= 76; caret.y += 15;
	fore = colors::yellow.mix(colors::black);
	textvalue(figure::Rect, get(Resources));
	fore = push_fore;
	caret = push_caret;
}

static void object_paint(const object* po) {
	auto p = po->data;
	if(bsdata<systemi>::have(p))
		((systemi*)p)->paint();
	else if(bsdata<planeti>::have(p))
		((planeti*)p)->paint();
}

void gamei::updateui() {
	draw::object::afterpaint = object_paint;
	add_planet(bsdata<planeti>::elements, {0, 0});
	add_system(bsdata<systemi>::elements, {0, 0});
}