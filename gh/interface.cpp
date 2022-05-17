#include "draw.h"
#include "draw_hexagon.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

const int size = 50;
static pathfind::indext current_index;
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
	draw::focusing(h2p(pt));
}

void creaturei::updateui() const {
	auto p = draw::findobject(this);
	if(!p) {
		auto pt = h2p(getposition());
		p = draw::addobject(pt.x, pt.y);
		p->data = this;
		p->resource = draw::getres(getid(), "art/creatures");
		p->priority = 4;
		splashscreen(500);
	}
}

static draworder* last_order;

void ordermove(void* object, point hp, int time, bool depended) {
	auto p = draw::findobject(object);
	if(!p)
		return;
	auto pt = h2p(hp);
	auto po = p->addorder(time, depended ? last_order : 0);
	po->x = pt.x;
	po->y = pt.y;
	last_order = po;
}

static draworder* floatstring(point pt, color fc, const char* format) {
	auto pb = draw::addobject(pt.x, pt.y);
	pb->string = szdup(format);
	pb->font = metrics::h1;
	pb->fore = fc;
	pb->alpha = 0;
	pb->priority = 101;
	auto po = pb->addorder(500);
	po->y -= size;
	po->start.alpha = 255;
	po->alpha = 255;
	po->set(draworder::AutoClear);
	return po;
}

static int calculate(int v1, int n, int m) {
	return v1 * n / m;
}

void indexable::fixattack(indexable& enemy) const {
	auto p1 = findobject(this);
	auto p2 = findobject(&enemy);
	if(!p1 || !p2)
		return;
	auto dx = p2->x - p1->x;
	auto dy = p2->y - p1->y;
	auto dz = isqrt(dx*dx + dy*dy);
	if(!dz)
		return;
	draworder* po;
	auto push_position = *p1;
	// Move enemy
	po = p1->addorder(150);
	po->x += calculate(size / 3, dx, dz);
	po->y += calculate(size / 3, dy, dz);
	po->wait();
	// Move back from enemy
	po = p1->addorder(250);
	po->x = push_position.x;
	po->y = push_position.y;
}

void indexable::fixdamage(int value) const {
	char temp[260]; stringbuilder sb(temp); sb.add("%1i", value);
	floatstring(h2p(getposition()), colors::red, temp);
}

void indexable::fixkill() const {
	auto p1 = findobject(this);
	if(!p1)
		return;
	auto po = p1->addorder(350);
	po->alpha = 0;
	po->set(draworder::AutoClear);
}

void indexable::fixmove(point hex) const {
	auto p1 = findobject(this);
	if(!p1)
		return;
	auto pt = h2p(hex); focusing(pt);
	auto po = p1->addorder(400);
	po->x = pt.x;
	po->y = pt.y;
	po->wait();
}