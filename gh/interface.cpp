#include "draw.h"
#include "draw_hexagon.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;
using namespace pathfind;

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
	auto dz = isqrt(dx * dx + dy * dy);
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

static void paint_grid() {
	rect rc = clipping; rc.offset(-32);
	char temp[260]; stringbuilder sb(temp);
	for(auto i = 0; i < hms * hms; i++) {
		auto hp = i2h(i); auto pt = h2p(hp) - camera;
		if(!pt.in(rc))
			continue;
		if(getmove(i) == Blocked)
			continue;
		sb.clear(); sb.add("%1i, %2i", hp.x, hp.y);
		caret.x = pt.x - textw(temp) / 2;
		caret.y = pt.y - texth() / 2;
		text(temp);
	}
}

static void circlefa(int size) {
	auto push_alpha = alpha;
	draw::alpha = 96;
	circlef(size);
	alpha = push_alpha;
}

static void textvalue(int value) {
	auto push_caret = caret;
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	caret.x -= (textw(temp) + 1) / 2;
	caret.y -= (texth() + 1) / 2;
	text(temp);
	caret = push_caret;
}

static void movemarker(indext index, int value) {
	auto push_fore = fore;
	auto radius = size * 4 / 10;
	fore = colors::button;
	if(ishilite(radius)) {
		fore = fore.mix(colors::white);
		if(hot.key == MouseLeft) {
			if(hot.pressed)
				fore = fore.mix(colors::white);
			else
				execute(buttonparam, index);
		}
	}
	circlefa(radius);
	fore = colors::button;
	circle(radius);
	fore = colors::text;
	textvalue(value);
	fore = push_fore;
}

static void paint_moverange() {
	auto push_caret = caret;
	rect rc = clipping; rc.offset(-32);
	char temp[260]; stringbuilder sb(temp);
	for(auto i = 0; i < hms * hms; i++) {
		auto hp = i2h(i); caret = h2p(hp) - camera;
		if(!caret.in(rc))
			continue;
		if(getmove(i) == Blocked)
			continue;
		movemarker(i, getmove(i));
	}
	caret = push_caret;
}

indext indexable::choosemove() {
	answers an;
	an.add((void*)Blocked, getnm("EndMove"));
	auto push_event = object::afterpaintall;
	object::afterpaintall = paint_moverange;
	splashscreen(500);
	auto result = an.choose();
	object::afterpaintall = push_event;
	return (indext)(int)result;
}

static void paint_ability_card() {
	auto pi = gres("cards", "art/objects");
	image(pi, 0, 0);
}

static void paint_statistic() {
	auto push_stroke = fore_stroke;
	auto push_font = font;
	auto push_width = width; width = 320;
	font = metrics::h1;
	caret.y += texth() / 3;
	fore_stroke = colors::active;
	texta(getnm("Attack"), AlignCenter | TextStroke);
	width = push_width;
	font = push_font;
	fore_stroke = push_stroke;
}

static void paint_cards() {
	auto push_caret = caret;
	caret.x = getwidth() - 320 * 2 - metrics::padding * 2 - metrics::border * 2;
	caret.y = 35;
	paint_ability_card();
	paint_statistic();
	caret = push_caret;
}

playercardi* playerdeck::choose(const char* title, bool need_remove, fnevent proc) {
	auto push_event = object::afterpaintall;
	object::afterpaintall = paint_cards;
	auto v = (int)deck::choose(title, bsdata<playercardi>::source, need_remove);
	object::afterpaintall = push_event;
	return bsdata<playercardi>::elements + v;
}