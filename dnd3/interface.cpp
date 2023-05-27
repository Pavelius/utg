#include "crt.h"
#include "creature.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "room.h"
#include "widget.h"

using namespace draw;

void initialize_png();

const int timer_seg = 1000;

static void copy(point& p1, const point& p2) {
	p1 = p2;
}

void roomi::paint() const {
	image(gres(avatar, "maps"), 0, flags);
}

void creature::paint() const {
	auto pa = gres(avatar, "art/avatars");
	imager(caret.x, caret.y, pa, 0, 32);
	circle(32);
}

static void object_painting(const object* pv) {
	if(bsdata<roomi>::have(pv->data))
		((roomi*)pv->data)->paint();
	else if(bsdata<creature>::have(pv->data))
		((creature*)pv->data)->paint();
}

void status_info() {
	auto push_height = height;
	height = metrics::padding * 2 + 64;
	gradv(colors::form.lighten(), colors::form.darken());
	caret.y += height;
	height = push_height;
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
			po->x = ps.x;
			po->y = ps.y;
		}
	}
}

static void update_creatures() {
	for(auto& e : bsdata<creature>()) {
		auto pd = findobject(&e);
		auto ps = e.getscreenc();
		if(!pd) {
			pd = addobject(ps, &e);
			pd->priority = 10;
			auto po = modify(pd);
			po->alpha = 255;
		} else if(ps != *pd) {
			auto po = modify(pd);
			copy(*po, ps);
			po->x = ps.x;
			po->y = ps.y;
		}
	}
}

void update_ui() {
	update_rooms();
	update_creatures();
	waitall();
}