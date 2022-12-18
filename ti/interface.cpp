#include "draw.h"
#include "draw_figure.h"
#include "draw_hexagon.h"
#include "draw_object.h"
#include "draw_marker.h"
#include "draw_strategy.h"
#include "main.h"

using namespace draw;

static bool need_update_ui = false;

const int button_height = 20;
const int size = 256;
static color player_colors[] = {
	{40, 40, 40},
	{97, 189, 79},
	{242, 214, 0},
	{255, 159, 26},
	{235, 90, 70},
	{195, 119, 224},
	{0, 121, 191},
};

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

static void texth2c(const char* format) {
	auto push_font = font;
	auto push_caret = caret;
	font = metrics::h2;
	caret.x -= textw(format) / 2;
	text(format);
	caret = push_caret;
	font = push_font;
}

static void text(const char* format, point offset, void(*proc)(const char*)) {
	auto push_caret = caret;
	caret = caret + offset;
	proc(format);
	caret = push_caret;
}

static void show_players() {
	auto push_y = caret.y;
	caret.x += 16; caret.y += 20;
	auto push_x = caret.x;
	auto res = gres("races_small");
	auto push_alpha = alpha;
	alpha = 32;
	for(auto p : game.players) {
		image(res, getbsi(p), 0);
		if(ishilite(16)) {
			hilite_object = p;
			hilite_size = 17;
			hilite_position = caret;
		}
		caret.x += 32;
	}
	alpha = push_alpha;
	auto pi = game.players.find(player);
	if(pi != -1) {
		auto p = game.players[pi];
		auto push_x1 = caret.x;
		caret.x = push_x + 32 * pi;
		image(res, getbsi(p), 0);
		auto push_fore = fore;
		caret.x = push_x1;
	}
	caret.y = push_y;
	caret.x -= 16;
}

static void textcnw(const char* name) {
	caret.x += (width - textw(name)) / 2;
	text(name);
}

static void status(const char* id, const char* value) {
	auto name = getnmsh(id);
	auto push_fore = fore;
	auto push_font = font;
	auto push_caret = caret;
	auto push_width = width;
	width = 32;
	fore = colors::border;
	line(caret.x, caret.y + 40);
	caret = push_caret;
	font = metrics::small;
	fore = colors::h3;
	textcnw(name);
	caret.y += texth();
	font = metrics::h2;
	fore = colors::text;
	caret.x = push_caret.x;
	textcnw(value);
	font = push_font;
	caret = push_caret;
	width = push_width;
	caret.x += 32;
	push_caret = caret;
	fore = colors::border;
	line(caret.x, caret.y + 40);
	caret = push_caret;
	fore = push_fore;
}

static void status(const char* id, int value) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value); status(id, temp);
}

static void status(indicator_s v) {
	status(bsdata<indicatori>::elements[v].id, player->get(v));
}

static void show_indicators() {
	static indicator_s source_tokes[] = {StrategyToken, FleetToken, TacticToken};
	static indicator_s source_goods[] = {TradeGoods, Commodities};
	static indicator_s source_score[] = {VictoryPoints};
	for(auto v : source_tokes)
		status(v);
	caret.x += 2;
	for(auto v : source_goods)
		status(v);
	caret.x += 2;
	status(getnmsh("Resources"), player->getplanetsummary(Resources));
	status(getnmsh("Influence"), player->getplanetsummary(Influence));
	caret.x += 2;
	for(auto v : source_score)
		status(v);
	caret.x += 2;
	status(getnmsh("ActionCards"), player->getcards());
	status(getnmsh("Technology"), player->gettechs());
}

void status_info(void) {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x += 4;
	show_players();
	fore = push_fore;
	show_indicators();
	caret = push_caret;
	caret.y += 4 * 2 + 32;
}

static void space_hexagon() {
	auto push_size = fsize; fsize = size;
	auto push_fore = fore; fore = colors::border;
	hexagon();
	fore = push_fore;
	fsize = push_size;
}

static void paint_special(int index, int frame) {
	auto push_caret = caret; caret = planet_position(caret, index);
	image(gres("tiles"), frame, 0);
	caret = push_caret;
}

static void special_paint(tile_s special, int special_index) {
	switch(special) {
	case NoSpecialTile: break;
	case WormholeAlpha: paint_special(special_index, special - 1); break;
	case WormholeBeta: paint_special(special_index, special - 1); break;
	default: paint_special(3, special - 1); break;
	}
}

void systemi::paint() const {
	special_paint(special, special_index);
	space_hexagon();
	auto push_caret = caret;
	caret.y -= size - 24;
	auto res = gres("races_small");
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
	height = button_height - 1;
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

static color getplayercolor(playeri* p) {
	return player_colors[game.origin_players.find(p) + 1];
}

void troop::paint(unsigned flags) const {
	auto push_color = fore;
	fore = getplayercolor(player);
	buttonback(43, 0);
	fore = push_color;
	textcn(getname());
}

static void windowed(const char* format) {
	rectpush push;
	swindow(false);
	textf(format);
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
	image(gres("planets"), frame, flags);
	text(getname(), {0, 56}, texth2c);
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
		image(gres("tech"), speciality - 1, 0);
	}
	if(trait) {
		const double cos = 0.7660444431189;
		const double sin = 0.6427876096865;
		const double radius = 74.0;
		caret = push_caret;
		caret.x -= (short)(radius * cos) * multiplier; caret.y -= (short)(radius * sin);
		image(gres("traits"), trait - 1, 0);
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
	else if(bsdata<marker>::have(p))
		((marker*)p)->paint();
}

static void add_planet(planeti* ps, point pt, int index) {
	auto p = addobject(pt.x, pt.y);
	p->priority = 2;
	p->data = ps;
	p->frame = ps->frame;
	switch(index) {
	case 1: p->flags |= ImageMirrorH; break;
	case 2: p->flags |= ImageMirrorV; break;
	}
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
	p->priority = 3;
	p->data = ps;
	add_planets(pt, ps);
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
		if(bsdata<marker>::have(e.data))
			e.clear();
	}
	bsdata<marker>::source.clear();
}

static void marker_press() {
	auto p = (draw::object*)hot.param;
	breakmodal((long)p->data);
}

static draw::object* add_maker(void* p, figure shape, int size, char priority = 50) {
	auto pd = findobject(p);
	if(!pd)
		return 0;
	auto pm = bsdata<marker>::add();
	pm->shape = shape;
	pm->size = size;
	pm->fore = colors::green;
	pm->data = p;
	auto ps = addobject(pd->x, pd->y);
	ps->data = pm;
	ps->priority = priority;
	return ps;
}

void entity::clear() {
	memset(this, 0, sizeof(*this));
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
	object::painting = object_paint;
	clearobjects();
	add_systems();
	draw::setcamera(h2p({0, 0}, size));
}

void gamei::focusing(const entity* p) {
	auto ps = p->getsystem();
	draw::slidecamera(h2p(i2h(ps->index), size), 48);
}

static void update_units(point position, const entity* location) {
	if(!location)
		return;
	entitya source;
	source.select(location);
	source.sortunit();
	if(!source)
		return;
	auto total_height = button_height * source.getcount();
	auto x = position.x;
	auto y = position.y - (total_height - button_height) / 2;
	for(auto pt : source) {
		auto p = findobject(pt);
		if(!p) {
			p = addobject(x, y);
			p->data = pt;
			p->priority = 15;
		}
		if(p->x != x || p->y != y) {
			auto po = p->addorder(1000);
			po->x = x;
			po->y = y;
		}
		y += button_height;
	}
}

void troop::updateui() {
	for(auto& e : bsdata<troop>()) {
		if(e)
			continue;
		auto p = findobject(&e);
		if(p)
			p->clear();
	}
}

void gamei::updateui() {
	static point system_offset = {0, -6 * size / 10};
	troop::updateui();
	waitall();
	for(auto& e : bsdata<object>()) {
		if(bsdata<planeti>::have(e.data))
			update_units(e, (entity*)e.data);
		else if(bsdata<systemi>::have(e.data))
			update_units(e + system_offset, (entity*)e.data);
	}
	waitall();
}