#include "draw.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

struct armyi {
	struct troop*	troop;
	char			level;
	draw::object*	object;
};
class armya : public adat<armyi> {
	bool have(const uniti* p) const {
		for(auto& e : *this) {
			if(e.troop->type == p)
				return true;
		}
		return false;
	}
	void add(troop* v, int level) {
		auto p = adat::add();
		p->troop = v;
		p->level = level;
		p->object = 0;
	}
	static int compare(const void* v1, const void* v2) {
		auto p1 = ((armyi*)v1);
		auto p2 = ((armyi*)v2);
		return p1->troop->type->abilities[Cost] - p2->troop->type->abilities[Cost];
	}
	void sort() {
		qsort(data, count, sizeof(data[0]), compare);
	}
public:
	void select(const entity* location) {
		clear();
		if(!location)
			return;
		for(auto& e : bsdata<troop>()) {
			if(e.location != location)
				continue;
			if(e.type->stackable() && have(e.type))
				continue;
			add(&e, 0);
		}
	}
};

//point points[6] = {
//	{(short)(caret.x + fsize * cos_30), (short)(caret.y - fsize / 2)},
//	{(short)(caret.x + fsize * cos_30), (short)(caret.y + fsize / 2)},
//	{(short)caret.x, (short)(caret.y + fsize)},
//	{(short)(caret.x - fsize * cos_30), (short)(caret.y + fsize / 2)},
//	{(short)(caret.x - fsize * cos_30), (short)(caret.y - fsize / 2)},
//	{(short)caret.x, (short)(caret.y - fsize)},
//};

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
	p->set(object::DisableInput);
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
	p->set(object::DisableInput);
}

static void add_troop(troop* ps, point pt) {
	auto p = addobject(pt.x, pt.y);
	p->data = ps;
	p->priority = 5;
	p->data = ps;
	p->priority = 10;
	p->size = 0;
}

void systemi::paint() const {
}

static void textcn(const char* format) {
	auto push_caret = caret;
	caret.x -= (textw(format) + 1) / 2;
	caret.y -= (texth() + 1) / 2;
	text(format);
	caret = push_caret;
}

static void textv(const char* format, ...) {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(format, xva_start(format));
	textcn(temp);
}

static void buttonback(int size) {
	auto push_alpha = alpha;
	rectpush push;
	width = size * 2;
	height = texth() + 1 * 2;
	caret.y -= (height + 1) / 2;
	caret.x -= (width + 1) / 2;
	alpha = 192;
	rectf();
	alpha = push_alpha;
	rectb();
}

void troop::paint() const {
	auto push_color = fore;
	fore = colors::red.mix(colors::black);
	buttonback(56);
	fore = push_color;
	if(type->abilities[CostCount] > 1)
		textv("%1 x%2i", getname(), 10);
	else
		textcn(getname());
}

static void background(figure shape, int size) {
	auto push_fore = fore;
	fore = colors::black;
	switch(shape) {
	case figure::Rect: field(figure::RectFill, size); break;
	case figure::Circle: field(figure::CircleFill, size); break;
	}
	fore = push_fore;
	field(shape, size);
}

static void textvalue(figure shape, const char* format) {
	background(shape, 12);
	textcn(format);
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
	caret.x -= 73; caret.y += 15;
	fore = colors::yellow.mix(colors::black);
	textvalue(figure::Circle, get(Resources));
	fore = push_fore;
	caret = push_caret;
}

static void object_paint(const object* po) {
	auto p = po->data;
	if(bsdata<systemi>::have(p))
		((systemi*)p)->paint();
	else if(bsdata<planeti>::have(p))
		((planeti*)p)->paint();
	else if(bsdata<troop>::have(p))
		((troop*)p)->paint();
}

static void update_system() {
	armya army;
	for(auto& e : bsdata<systemi>()) {
		if(!e)
			continue;
		army.clear();
		army.select(&e);
	}
}

static point planet_position(point caret, int index) {
	const double cos_30 = 0.86602540378;
	auto fsize = 6 * size / 10;
	point position[] = {
		{(short)(caret.x + fsize * cos_30), (short)(caret.y - fsize / 2)},
		{(short)(caret.x - fsize * cos_30), (short)(caret.y + fsize / 2)},
		{(short)(caret.x + fsize / 2), (short)(caret.y + 2 * fsize / 3)},
	};
	return position[index];
}

void gamei::updateui() {
	draw::object::afterpaint = object_paint;
	add_planet(bsdata<planeti>::elements + 0, planet_position({0, 0}, 0));
	add_planet(bsdata<planeti>::elements + 1, planet_position({0, 0}, 1));
	add_planet(bsdata<planeti>::elements + 2, planet_position({0, 0}, 2));
	add_system(bsdata<systemi>::elements, {0, 0});
	add_troop(bsdata<troop>::elements + 0, {0, -8});
	add_troop(bsdata<troop>::elements + 1, {0, 13});
	add_troop(bsdata<troop>::elements + 2, {21, -64});
	add_troop(bsdata<troop>::elements + 3, {0, -85});
}