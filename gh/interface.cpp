#include "draw.h"
#include "draw_hexagon.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

const int size = 50;
static indext current_index;
static bool show_movement_cost, show_hex_grid, show_hex_coor;

static point h2p(point v) {
	return draw::h2p(v, size);
}

static point p2h(point v) {
	return draw::p2h(v, size);
}

void scenariotilei::updateui() const {
	auto p = draw::findobject(this);
	if(!p) {
		auto pt = h2p(position) - type->offset;
		p = draw::addobject(pt.x, pt.y);
		p->data = this;
		p->resource = draw::getres(type->id, "art/tiles");
		p->priority = type->priority;
		if(inverse)
			p->flags = ImageMirrorH | ImageMirrorV;
	}
}

void gamei::setcamera(point pt) {
	draw::setcamera(h2p(pt));
}