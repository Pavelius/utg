#include "crt.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "resable.h"
#include "room.h"
#include "widget.h"

using namespace draw;

void initialize_png();

const int timer_seg = 1000;

static void copy(point& p1, const point& p2) {
	p1 = p2;
}

void resable::paint() const {
	image(gres(avatar, "maps"), 0, flags);
}

static void object_painting(const object* pv) {
	if(bsdata<roomi>::have(pv->data))
		((roomi*)pv->data)->paint();
}

static void ui_background() {
	strategy_background();
	paintobjects();
}

void initialize_ui() {
	initialize_png();
	pbackground = ui_background;
	object::painting = object_painting;
}

static draworder* modify(object* po) {
	return po->addorder(1000);
}

static void update_rooms() {
	for(auto& e : bsdata<roomi>()) {
		auto pd = findobject(&e);
		auto ps = e.getscreen();
		if(!pd) {
			pd = addobject(ps, &e);
			auto po = modify(pd);
			po->alpha = 255;
		} else if(ps != *pd) {
			auto po = modify(pd);
			copy(*po, ps);
		}
	}
}

void update_ui() {
	update_rooms();
	waitall();
}