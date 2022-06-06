#include "draw.h"
#include "draw_hexagon.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

const int button_height = 21;
const int size = 256;
static color player_colors[] = {
	{97, 189, 79},
	{242, 214, 0},
	{255, 159, 26},
	{235, 90, 70},
	{195, 119, 224},
	{0, 121, 191},
};

struct armyi {
	struct troop*	troop;
	char			level;
};
static const entity* last_parent;
class armya : public adat<armyi, 32> {
	static int compare_unit(const void* v1, const void* v2) {
		auto p1 = (uniti*)v1;
		auto p2 = (uniti*)v2;
		if(p1->type != p2->type)
			return p2->type - p1->type;
		if(p1->abilities[Cost] != p2->abilities[Cost])
			return p2->abilities[Cost] - p1->abilities[Cost];
		return p1 - p2;
	}
	static int compare_troop(const void* v1, const void* v2) {
		auto p1 = (troop*)v1;
		auto p2 = (troop*)v2;
		auto u1 = p1->type;
		auto u2 = p2->type;
		if(u1 == u2)
			return p1 - p2;
		return compare_unit(u1, u2);
	}
	static entity* getcomparer(entity* p) {
		return (p->location == last_parent) ? p : p->location;
	}
	static int compare_tree(const void* v1, const void* v2) {
		auto t1 = ((armyi*)v1)->troop;
		auto t2 = ((armyi*)v2)->troop;
		auto c1 = getcomparer(t1);
		auto c2 = getcomparer(t2);
		if(c1 == c2) {
			if(t2->location == last_parent)
				return 1;
			return compare_troop(t1, t2);
		}
		return compare_troop(c1, c2);
	}
public:
	void selectnew(const entity* location) {
		auto ps = data;
		auto pe = endof();
		for(auto& e : bsdata<troop>()) {
			if(e.location == location) {
				if(ps < pe) {
					ps->troop = &e;
					ps->level = 0;
					ps++;
				}
			} else if(bsdata<troop>::have(e.location) && ((troop*)e.location)->location == location) {
				if(ps < pe) {
					ps->troop = &e;
					ps->level = 1;
					ps++;
				}
			}
		}
		count = ps - data;
	}
	void sortnew(const entity* location) {
		last_parent = location;
		qsort(data, count, sizeof(data[0]), compare_tree);
	}
	void select(const entity* location) {
		clear();
		if(!location)
			return;
		selectnew(location);
		sortnew(location);
	}
};

static void show_players() {
	auto push_y = caret.y;
	caret.x += 16; caret.y += 16;
	auto push_x = caret.x;
	auto res = getres("races_small");
	for(auto p : game.players) {
		image(res, getbsi(p), 0);
		if(ishilite(17)) {
			hilite_type = figure::Circle;
			hilite_object = p;
			hilite_size = 18;
			hilite_position = caret;
		}
		caret.x += 32 + 8;
	}
	auto pi = game.players.find(game.active);
	if(pi != -1) {
		auto push_x1 = caret.x;
		caret.x = push_x + (32 + 8) * pi;
		auto push_fore = fore;
		fore = colors::active;
		circle(17);
		fore = push_fore;
		caret.x = push_x1;
	}
	caret.y = push_y;
}

static void status(const char* id, const char* value) {
	caret.x += 2;
	auto name = getnm(id);
	auto push_fore = fore;
	fore = colors::h3;
	text(name); caret.x += textw(name); text(":"); caret.x += textw(":") + 2;
	fore = colors::text;
	text(value); caret.x += textw(value) + 2;
	fore = push_fore;
}

static void status(const char* id, int value) {
	char temp[32]; stringbuilder sb(temp);
	sb.add(value); status(id, temp);
}

static void status(indicator_s v) {
	status(bsdata<indicatori>::elements[v].id, game.active->get(v));
}

static void show_indicators() {
	static indicator_s source[] = {StrategyToken, FleetToken, TacticToken};
	for(auto v : source)
		status(v);
}

void status_info(void) {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x += 4;
	caret.y += 4;
	show_players();
	fore = push_fore;
	show_indicators();
	caret = push_caret;
	caret.y += 4 * 4 + 24;
}

void systemi::paint() const {
	auto push_caret = caret;
	caret.y -= size - 24;
	auto res = getres("races_small");
	for(auto p : game.players) {
		if(isactivated(p)) {
			image(res, getbsi(p), 0);
			caret.x += 30;
			caret.y += 16;
		}
	}
	caret = push_caret;
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

static void buttonback(int size, void* data) {
	auto push_alpha = alpha;
	rectpush push;
	width = size * 2;
	height = texth() + 1 * 2;
	caret.y -= (height + 1) / 2;
	caret.x -= (width + 1) / 2;
	alpha = (192 * push_alpha) >> 8;
	rectf();
	alpha = push_alpha;
	rectb();
	if(data && ishilite()) {
		hilite_object = data;
		if(hot.key == MouseLeft && !hot.pressed)
			execute(buttonparam, (long)data);
	}
}

void troop::paint(unsigned flags) const {
	auto push_color = fore;
	auto i = game.origin_players.find(player);
	if(i == -1)
		i = 0;
	fore = player_colors[i];
	buttonback(43, 0);
	fore = push_color;
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
	if(speciality) {
		caret = push_caret;
		caret.x -= 73 * multiplier; caret.y -= 15;
		image(getres("tech"), speciality - 1, 0);
	}
	if(trait) {
		const double cos = 0.7660444431189;
		const double sin = 0.6427876096865;
		const double radius = 74.0;
		caret = push_caret;
		caret.x -= (short)(radius * cos) * multiplier; caret.y -= (short)(radius * sin);
		image(getres("traits"), trait - 1, 0);
	}
	fore = push_fore;
	caret = push_caret;
	if(player)
		image(gres("races_small", "art/objects"), getbsi(player), 0);
}

static void object_paint(const object* po) {
	auto p = po->data;
	if(bsdata<systemi>::have(p))
		((systemi*)p)->paint();
	else if(bsdata<planeti>::have(p))
		((planeti*)p)->paint(po->flags);
	else if(bsdata<troop>::have(p))
		((troop*)p)->paint(po->flags);
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
		{caret}
	};
	return position[index];
}

static void add_planet(planeti* ps, point pt, int index) {
	auto p = addobject(pt.x, pt.y);
	p->priority = 2;
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

static void add_special(point pt, int index, int frame) {
	pt = planet_position(pt, index);
	auto p = addobject(pt.x, pt.y);
	p->priority = 2;
	p->size = 0;
	p->set(object::DisableInput);
	p->resource = getres("tiles");
	p->frame = frame;
}

static void add_system(systemi* ps, point pt) {
	auto p = addobject(pt.x, pt.y);
	p->priority = 3;
	p->data = ps;
	p->shape = figure::Hexagon;
	p->size = size;
	p->fore = colors::border;
	p->set(object::DisableInput);
	add_planets(pt, ps);
	switch(ps->special) {
	case NoSpecialTile: break;
	case WormholeAlpha: add_special(pt, ps->special_index, ps->special - 1); break;
	case WormholeBeta: add_special(pt, ps->special_index, ps->special - 1); break;
	default: add_special(pt, 3, ps->special - 1); break;
	}
}

static void add_systems() {
	auto push_interactive = answers::interactive;
	for(auto& e : bsdata<systemi>()) {
		if(!e.isplay())
			continue;
		add_system(&e, draw::h2p(i2h(e.index), size));
	}
	answers::interactive = push_interactive;
}

static void remove_all_markers() {
	for(auto& e : bsdata<object>()) {
		if(bsdata<object>::have(e.data))
			e.clear();
	}
}

static void marker_press() {
	auto p = (draw::object*)hot.param;
	breakmodal((long)p->data);
}

static draw::object* add_maker(void* p, figure shape, int size, char priority = 50) {
	auto pd = findobject(p);
	if(!pd)
		return 0;
	auto ps = addobject(pd->x, pd->y);
	ps->data = pd;
	ps->priority = priority;
	ps->shape = figure::Circle;
	ps->input = marker_press;
	ps->size = size;
	ps->fore = colors::green;
	ps->set(object::Hilite);
	return ps;
}

troop* gamei::choosetroop(const entitya& source) {
	answers an;
	auto result = an.choose(0, getnm("Cancel"), 1);
	return (troop*)result;
}

systemi* gamei::choosesystem(const entitya& source) {
	draw::pause();
	for(auto p : source)
		add_maker(p, figure::Circle, size / 3);
	answers an;
	auto result = an.choose(0, getnm("Cancel"), 1);
	remove_all_markers();
	return (systemi*)result;
}

void gamei::prepareui() {
	object::afterpaint = object_paint;
	clearobjects();
	add_systems();
	draw::setcamera(h2p({0, 0}, size));
}

void gamei::focusing(const entity* p) {
	auto ps = p->getsystem();
	draw::slidecamera(h2p(i2h(ps->index), size), 48);
}

static void update_units(point position, const entity* location) {
	armya source;
	source.select(location);
	if(!source)
		return;
	auto total_height = button_height * source.getcount();
	auto y = position.y - total_height / 2 + (button_height - 1) / 2;
	for(auto& e : source) {
		auto x = position.x + e.level * (button_height - 4);
		auto p = findobject(e.troop);
		if(!p) {
			p = addobject(x, y);
			p->data = e.troop;
			p->priority = 5;
			p->size = 0;
		}
		if(p->x != x || p->y != y) {
			auto po = p->addorder(1000);
			po->x = x;
			po->y = y;
		}
		y += button_height;
	}
}

void gamei::updateui() {
	static point system_offset = {0, -6 * size / 10};
	waitall();
	for(auto& e : bsdata<object>()) {
		if(bsdata<planeti>::have(e.data)) {
			if(equal("P0p0p0", ((entity*)e.data)->id))
				update_units(e, (entity*)e.data);
			else
				update_units(e, (entity*)e.data);
		} else if(bsdata<systemi>::have(e.data))
			update_units(e + system_offset, (entity*)e.data);
	}
	waitall();
}