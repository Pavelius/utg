#include "draw.h"
#include "draw_object.h"
#include "draw_hexagon.h"
#include "main.h"

using namespace pathfind;
using namespace draw;

const int size = 42;
const int appear_pause = 600;

static object* marker_object;

static point i2s(indext i) {
	short x = oceani::getx(i);
	short y = oceani::gety(i);
	return draw::fh2p({x, y}, size);
}

static void add_camera() {
	draw::camera = {-220, -90};
}

static void add_seamap() {
	auto p = draw::addobject(192, 203);
	p->resource = draw::getres("seamap");
	p->priority = 10;
}

static void add_warning(point pt) {
	auto p = draw::addobject(pt.x - 16, pt.y + 12);
	p->resource = draw::getres("tiles");
	p->priority = 21;
	p->frame = 36;
}

static void add_you_ship() {
	auto index = game.getmarker();
	if(index == Blocked)
		return;
	auto pt = i2s(index);
	marker_object = draw::addobject(pt.x, pt.y);
	marker_object->resource = draw::getres("tiles");
	marker_object->frame = 31;
	marker_object->priority = 29;
}

static void add_tile(point pt, const sprite* resource, int frame, const void* data) {
	auto p = draw::addobject(pt.x, pt.y);
	p->resource = resource;
	p->frame = frame;
	p->size = 2 * size / 3;
	p->priority = 20;
	p->data = (void*)data;
	p->fore = colors::border;
}

static void add_select(point pt, indext data) {
	auto p = draw::addobject(pt.x, pt.y);
	p->size = 2 * size / 3;
	p->priority = 30;
	p->shape = figure::Circle;
	p->data = (void*)data;
	p->fore = colors::button;
	p->set(object::Hilite);
}

static void add_select(indext index) {
	auto x = oceani::getx(index);
	auto y = oceani::gety(index);
	add_select(fh2p({x, y}, size), index + 1);
}

void oceani::showseamap() {
	createobjects();
	showobjects();
}

void oceani::showindecies() {
	char temp[4096]; stringbuilder sb(temp);
	clearobjects();
	add_camera();
	add_seamap();
	for(short y = 0; y < map_y; y++) {
		for(short x = 0; x < map_x; x++) {
			auto i = getindex(x, y);
			auto pt = draw::fh2p({x, y}, size);
			auto p = draw::addobject(pt.x, pt.y);
			p->size = 2 * size / 3;
			p->priority = 20;
			p->string = sb.get();
			sb.add("%1i", i);
			sb.addsz();
			p->fore = colors::border;
		}
	}
	chooseobject();
}

void oceani::createobjects() {
	marker_object = 0;
	clearobjects();
	add_camera();
	add_you_ship();
	add_seamap();
	auto tiles = getres("tiles");
	for(auto& e : bsdata<tilei>()) {
		if(e.is(Discarded))
			continue;
		if(e.index == Blocked)
			continue;
		auto x = getx(e.index);
		auto y = gety(e.index);
		auto pt = draw::fh2p({x, y}, size);
		add_tile(pt, tiles, e.frame, &e);
		if(e.is(StopMovement))
			add_warning(pt);
	}
}

indext oceani::choose(const char* title) const {
	int pv = (int)chooseobject();
	if(!pv)
		return Blocked;
	return pv - 1;
}

static void blockwalls() {
	for(auto& e : bsdata<tilei>()) {
		if(e.isactive() && e.isblocked())
			setmove(e.index, Blocked);
	}
}

static void blockopentiles() {
	for(auto& e : bsdata<tilei>()) {
		if(e.isactive() && e.isnavigation())
			blocknearest(e.index, 1);
	}
}

static void blockalltiles() {
	for(auto& e : bsdata<tilei>()) {
		if(e.isactive())
			setmove(e.index, Blocked);
	}
}

void oceani::createselections(int from, int to) const {
	for(auto i = 0; i < maxcount; i++) {
		auto m = getmove(i);
		if(m >= from && m <= to)
			add_select(i);
	}
}

void oceani::addpossiblecourse() const {
	clearpath();
	blockwalls();
	blockopentiles();
	blocknearest(marker, 1);
	blockalltiles();
	createselections(1, 1);
}

void oceani::addpossiblemove(int range) const {
	clearpath();
	blockwalls();
	makewave(marker);
	createselections(1, range);
}

void oceani::showsplash() {
	splashscreen(appear_pause);
}

indext oceani::chooseroute(const char* title, int range) const {
	createobjects();
	addpossiblemove(range);
	splashscreen(appear_pause);
	auto result = choose(0);
	if(result == Blocked)
		return Blocked;
	if(marker_object)
		marker_object->move(i2s(result), 2);
	return result;
}

void oceani::initialize() {
	pathfind::maxcount = map_x * map_y;
	pathfind::to = to;
}

static point getdirection(point hex, int direction) {
	static point directions[2][6] = {
		{{1, 0}, {0, 1}, {-1, 0}, {-1, -1}, {0, -1}, {1, -1}},
		{{1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {0, -1}, {1, 0}},
	};
	auto parity = hex.x & 1;
	auto offset = directions[parity][direction];
	return hex + offset;
}

indext oceani::to(indext i, int direction) {
	auto x = getx(i), y = gety(i);
	auto pt = getdirection({x, y}, direction);
	if(pt.x < 0 || pt.y < 0 || pt.x >= map_x || pt.y >= map_y)
		return Blocked;
	return getindex(pt.x, pt.y);
}

bool tilei::moveto(indext goal, int bonus) {
	clearpath();
	blockwalls();
	makewave(index);
	indext path[map_x * map_y];
	auto count = getpath(index, goal, path, sizeof(path) / sizeof(path[0]));
	if(!count)
		return false;
	oceani::createobjects();
	auto p = findobject(this);
	if(!p)
		return false;
	splashscreen(appear_pause);
	while(count-- > 0 && bonus-- > 0) {
		index = path[count];
		p->move(i2s(index), appear_pause);
	}
	return true;
}

point oceani::gethexsize() {
	const double cos_30 = 0.86602540378;
	return {size * 2, (short)(2.0 * size * cos_30)};
}