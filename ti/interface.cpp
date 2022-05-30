#include "draw.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

struct armyi {
	struct troop*	troop;
	char			level;
};
class armya : public adat<armyi, 32> {
	void add(troop* v, int level) {
		auto p = adat::add();
		p->troop = v;
		p->level = level;
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
	static bool find(const armyi* pb, const armyi* ps, const uniti* type) {
		for(auto pv = pb; pv < ps; pv++) {
			if(pv->troop->type == type)
				return true;
		}
		return false;
	}
	void selectbasic(const entity* location) {
		auto ps = data;
		auto pe = endof();
		for(auto& e : bsdata<troop>()) {
			if(e.location != location)
				continue;
			if(e.type->stackable() && find(data, ps, e.type))
				continue;
			if(ps < pe) {
				ps->troop = &e;
				ps->level = 0;
			}
		}
		count = ps - data;
	}
	void selectsibling(armya& source) {
		auto ps = data;
		for(auto& e : source) {
			*ps++ = e;
			auto pb = ps;
			for(troop* p = e.troop->sibling(0); p; p = e.troop->sibling(p)) {
				if(e.troop->type->stackable()) {
					auto found = false;
					for(auto pv = pb; pv < ps; pv++) {
						if(pv->troop->type == p->type) {
							found = true;
							break;
						}
					}
					if(found)
						continue;
				}
				ps->troop = p;
				ps->level = e.level + 1;
				ps++;
			}
		}
		count = ps - data;
	}
	void select(const entity* location) {
		clear();
		if(!location)
			return;
		armya basic;
		basic.clear();
		basic.select(location);
		basic.sort();
		selectsibling(basic);
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

void planeti::paint(unsigned flags) const {
	auto push_caret = caret;
	auto push_fore = fore;
	auto multiplier = -1;
	if(flags & ImageMirrorH)
		multiplier = 1;
	caret.x -= 58 * multiplier; caret.y += 42;
	fore = colors::blue.mix(colors::white);
	textvalue(figure::Circle, get(Influence));
	caret = push_caret;
	caret.x -= 73 * multiplier; caret.y += 15;
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
		((planeti*)p)->paint(po->flags);
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

static point system_position(point caret, int index) {
	switch(index) {
	case 1: return caret + point{size / 3, -2 * size / 3};
	default: return caret + point{-size / 3, -2 * size / 3};
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

static void add_planet(planeti* ps, point pt, int index) {
	auto p = addobject(pt.x, pt.y);
	p->priority = 5;
	p->data = ps;
	p->frame = ps->frame;
	p->resource = gres("planets", "art/objects");
	p->size = 56;
	p->font = metrics::h2;
	p->string = ps->getname();
	switch(index) {
	case 1: p->flags |= ImageMirrorH; break;
	case 2: p->flags |= ImageMirrorV; break;
	}
	p->set(object::DisableInput);
}

static void add_planets(point pt, const systemi* ps) {
	auto index = 0;
	entitya planets;
	planets.selectplanets(ps);
	auto maximum = planets.getcount();
	for(auto pe : planets) {
		auto p = (planeti*)pe;
		if(maximum < 2)
			add_planet(p, pt, false);
		else if(index < 3)
			add_planet(p, planet_position(pt, index), index);
		index++;
	}
}

static void add_system(systemi* ps, point pt) {
	auto p = addobject(pt.x, pt.y);
	p->priority = 1;
	p->data = ps;
	p->shape = figure::Hexagon;
	p->size = size;
	p->fore = colors::border;
	p->set(object::DisableInput);
	add_planets(pt, ps);
}

void gamei::prepareui() {
	draw::object::afterpaint = object_paint;
	add_system(game.active->gethome(), {0, 0});
}

static void update_army(entity* location) {
	auto ps = findobject(location);
	if(!ps)
		return;
	armya army;
	army.select(location);
}

static void update_units_system() {
	entitya source;
	for(auto& e : bsdata<troop>()) {
	}
}

static void update_units() {
}

void gamei::updateui() {
	add_troop(bsdata<troop>::elements + 0, {0, -8});
	add_troop(bsdata<troop>::elements + 1, {0, 13});
	add_troop(bsdata<troop>::elements + 2, {21, -64});
	add_troop(bsdata<troop>::elements + 3, {0, -85});
}