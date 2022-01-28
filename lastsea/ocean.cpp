#include "draw.h"
#include "drawobject.h"
#include "draw_hexagon.h"
#include "main.h"

using namespace pathfind;
using namespace draw;

const int size = 42;

static point i2s(indext i) {
	short x = oceani::getx(i);
	short y = oceani::gety(i);
	return draw::fh2p({x, y}, size);
}

static void add_camera() {
	draw::camera = {220, 102};
}

static void add_seamap() {
	auto p = draw::addobject(192, 198);
	p->resource = draw::getres("seamap");
	p->priority = 10;
}

static void add_you_ship() {
	auto pt = i2s(game.getmarker());
	auto p = draw::addobject(pt.x, pt.y);
	p->resource = draw::getres("tiles");
	p->frame = 1;
	p->priority = 29;
}

static void add_tile(point pt, const sprite* resource, int frame, const void* data) {
	auto p = draw::addobject(pt.x, pt.y);
	p->resource = resource;
	p->frame = frame;
	p->size = 2 * size / 3;
	p->priority = 20;
	p->data = (void*)data;
	p->setcolorborder();
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

void oceani::update() const {
	auto tiles = getres("tiles");
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
			auto fi = 0;
			if(t == Blocked)
				add_tile(pt, tiles, fi, 0);
			else
				add_tile(pt, tiles, fi, data + i);
		}
	}
}

void oceani::update(const slice<indext>& selectable) const {
	for(auto v : selectable) {
		short x = getx(v);
		short y = gety(v);
		auto pt = draw::fh2p({x, y}, size);
		add_select(pt, data + v);
	}
}

indext oceani::choose(const char* title) const {
	update();
	auto pv = chooseobject();
	if(!pv)
		return 0xFFFF;
	return (indext*)pv - data;
}

indext oceani::choose(const char* title, const slice<indext>& selectable) const {
	update();
	update(selectable);
	auto pv = chooseobject();
	if(!pv)
		return 0xFFFF;
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

void oceani::paintcourse() const {
	indext coodrinates[32];
	clearpath();
	blockwalls();
	blockopentiles();
	blocknearest(marker, 1);
	blockalltiles();
	auto count = getindeciesat(coodrinates, lastof(coodrinates), 1);
	if(!count)
		return;
	update();
	update({coodrinates, count});
	splashscreen();
}

indext oceani::chooseindex(const char* title) const {
	auto pv = chooseobject();
	if(!pv)
		return 0xFFFF;
	return (indext*)pv - data;
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
			p->setcolorborder();
		}
	}
	chooseobject();
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