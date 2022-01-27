#include "drawobject.h"
#include "draw_hexagon.h"
#include "main.h"

using namespace draw;

const int size = 42;

static void add_camera() {
	draw::camera = {220, 102};
}

static void add_seamap() {
	auto p = draw::addobject(192, 198);
	p->resource = draw::getres("seamap");
	p->priority = 10;
}

void oceani::update() const {
	auto tiles = getres("tiles");
	clearobjects();
	add_camera();
	add_seamap();
	for(short y = 0; y < my; y++) {
		for(short x = 0; x < mx; x++) {
			auto i = gi(x, y);
			auto t = getlocation(i);
			if(!t)
				continue;
			auto pt = draw::fh2p({x, y}, size);
			auto p = draw::addobject(pt.x, pt.y);
			p->resource = tiles;
			p->frame = t - 1;
			p->size = 2 * size / 3;
			p->priority = 20;
			p->data = (void*)(data + i);
			p->setcolorborder();
		}
	}
}

indext oceani::choose(const char* title) const {
	update();
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
			auto i = gi(x, y);
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