#include "draw.h"
#include "drawobject.h"
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
	draw::camera = {220, 90};
}

static void add_seamap() {
	auto p = draw::addobject(192, 203);
	p->resource = draw::getres("seamap");
	p->priority = 10;
}

static void add_you_ship() {
	auto pt = i2s(game.getmarker());
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

static void add_select(point pt, const void* data) {
	auto p = draw::addobject(pt.x, pt.y);
	p->size = 2 * size / 3;
	p->priority = 30;
	p->shape = figure::Circle;
	p->data = (void*)data;
	p->fore = colors::button;
	p->set(object::Hilite);
}

static void add_select(indext index, const void* data) {
	auto x = oceani::getx(index);
	auto y = oceani::gety(index);
	add_select(fh2p({x, y}, size), data);
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
	for(short y = 0; y < my; y++) {
		for(short x = 0; x < mx; x++) {
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

void oceani::createobjects() const {
	auto tiles = getres("tiles");
	marker_object = 0;
	clearobjects();
	add_camera();
	add_you_ship();
	add_seamap();
	for(short y = 0; y < my; y++) {
		for(short x = 0; x < mx; x++) {
			auto i = getindex(x, y);
			auto t = getlocation(i);
			if(!t)
				continue;
			auto pt = draw::fh2p({x, y}, size);
			if(t == Blocked)
				add_tile(pt, tiles, 0, data + i);
			else if(t <= 30)
				add_tile(pt, tiles, t, data + i);
			else {
				auto po = tilei::find(t);
				add_tile(pt, tiles, po ? po->frame : 0, data + i);
			}
		}
	}
}

indext oceani::choose(const char* title) const {
	auto pv = chooseobject();
	if(!pv)
		return Blocked;
	return (indext*)pv - data;
}

void oceani::blockwalls() const {
	for(auto i = 0; i < mx * my; i++) {
		if(data[i] == Blocked)
			setmove(i, Blocked);
	}
}

void oceani::blockopentiles() const {
	for(auto i = 0; i < mx * my; i++) {
		if(data[i] >= 1 && data[i] <= 30)
			blocknearest(i, 1);
	}
}

void oceani::blockalltiles() const {
	for(auto i = 0; i < mx * my; i++) {
		if(data[i])
			setmove(i, Blocked);
	}
}

void oceani::createselections(int from, int to) const {
	for(auto i = 0; i < maxcount; i++) {
		auto m = getmove(i);
		if(m >= from && m <= to)
			add_select(i, data + i);
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
	pathfind::maxcount = mx * my;
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
	if(pt.x < 0 || pt.y < 0 || pt.x >= mx || pt.y >= my)
		return Blocked;
	return getindex(pt.x, pt.y);
}

indext oceani::getindex(const void* p) const {
	if(p >= data && p < ((char*)data + sizeof(data)))
		return (indext*)p - data;
	return Blocked;
}

indext oceani::findindex(indext v) {
	for(auto& e : data) {
		if(e == v)
			return &e - data;
	}
	return Blocked;
}

bool oceani::stepto(indext start, indext goal) {
	auto p = findobject(data + start);
	if(!p)
		return false;
	indext path[mx * my];
	clearpath();
	blockwalls();
	makewave(start);
	auto count = getpath(start, goal, path, sizeof(path) / sizeof(path[0]));
	if(!count)
		return false;
	p->move(i2s(path[count - 1]), appear_pause);
	return true;
}

point oceani::gethexsize() {
	const double cos_30 = 0.86602540378;
	return {size * 2, (short)(2.0 * size * cos_30)};
}