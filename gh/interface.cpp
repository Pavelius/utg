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
	if(p)
		return;
	auto ps = bsdata<tilei>::find(type);
	if(ps) {
		ps->creating(position, inverse);
		auto pt = h2p(position) - ps->offset;
		p = draw::addobject(pt.x, pt.y);
		p->data = this;
		p->resource = draw::getres(type, "art/tiles");
		p->priority = ps->priority;
		if(inverse)
			p->flags = ImageMirrorH | ImageMirrorV;
	}
	auto pm = bsdata<monsteri>::find(type);
	if(pm) {
		auto p = creaturei::add(type, position, false);
		p->set(Hostile);
		p->updateui();
	}
}

void gamei::focusing(point pt) {
	draw::focusing(h2p(pt));
}

void creaturei::updateui() const {
	auto p = draw::findobject(this);
	if(!p) {
		auto pt = h2p(getposition());
		focusing(pt);
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

static void fixvalue(point hex, int value, color fore) {
	if(!value)
		return;
	char temp[260]; stringbuilder sb(temp); sb.add("%1i", value);
	floatstring(h2p(hex), fore, temp);
}

void indexable::fixdamage(int value) const {
	fixvalue(getposition(), value, colors::red);
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

void indexable::fixexperience(int value) const {
	fixvalue(getposition(), value, colors::yellow);
}

void indexable::fixheal(int value) const {
	fixvalue(getposition(), value, colors::green);
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

static void textvalue(const char* value, unsigned flags = TextStroke) {
	auto push_caret = caret;
	caret.x -= (textw(value) + 1) / 2;
	caret.y -= (texth() + 1) / 2;
	text(value, -1, flags);
	caret = push_caret;
}

static void textvalueng(const char* value, unsigned flags) {
	auto push_fore = fore;
	fore = colors::black;
	textvalue(value, flags);
	fore = push_fore;
}

static void textvalue(int value) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	textvalue(temp);
}

static void textvalueng(int value) {
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i", value);
	textvalueng(temp, 0);
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

static void targetmarker(const void* param, color v) {
	auto push_fore = fore;
	auto radius = size * 4 / 10;
	fore = v;
	if(ishilite(radius)) {
		fore = fore.mix(colors::white);
		if(hot.key == MouseLeft) {
			if(hot.pressed)
				fore = fore.mix(colors::white);
			else
				execute(buttonparam, (long)param);
		}
	}
	circlefa(radius);
	fore = v;
	circle(radius);
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

static void paint_targets() {
	auto push_caret = caret;
	for(auto p : *creaturea::last) {
		if(!(*p))
			continue;
		auto hp = i2h(p->getindex()); caret = h2p(hp) - camera;
		targetmarker(p, colors::red);
	}
	caret = push_caret;
}

creaturei* creaturea::choose(const char* title) const {
	last = this;
	answers an;
	auto push_event = object::afterpaintall;
	object::afterpaintall = paint_targets;
	splashscreen(500);
	auto result = (creaturei*)an.choose(title, getnm("Cancel"), 1);
	object::afterpaintall = push_event;
	return result;
}

static void paint_ability_card(color card_color) {
	auto push_fore = fore;
	auto push_width = width;
	auto push_height = height;
	auto push_alpha = alpha;
	width = 320; height = 436;
	fore = card_color;
	rectf();
	auto pi = gres("cards", "art/objects");
	alpha = 216;
	image(pi, 0, 0);
	alpha = push_alpha;
	width = push_width;
	height = push_height;
	fore = push_fore;
}

static void fix_fiscard(point origin, int mode) {
	auto pi = gres("cards", "art/objects");
	auto pt = origin; pt.x += 78; pt.y += 104;
	if(!mode) {
		pt.x += 18;
		pt.y -= 2;
	}
	image(pt.x, pt.y, pi, 2, 0);
}

static void paint_effect(variants effect, point origin, int mode) {
	char temp[260]; stringbuilder sb(temp);
	auto push_font = font;
	for(auto v : effect) {
		if(v.iskind<actioni>() && v.value == Discard) {
			fix_fiscard(origin, mode);
			continue;
		}
		sb.clear();
		auto pn = v.getname();
		if(szfind(pn, "%1i") || (!v.iskind<actioni>() && !v.iskind<modifieri>()))
			sb.add(pn, v.counter);
		else
			sb.add("%1% %2i", v.getname(), v.counter);
		if(v.iskind<actioni>() || v.iskind<summoni>() || v.iskind<trapi>() || v.iskind<statei>())
			font = metrics::h2;
		else
			font = metrics::font;
		auto push_caret = caret;
		caret.x -= (textw(temp) + 1) / 2;
		text(temp, -1);
		caret = push_caret;
		caret.y += texth();
	}
	font = push_font;
}

static int get_height(variants effect) {
	auto push_clip = clipping; clipping.clear();
	auto push_caret = caret;
	paint_effect(effect, {}, 0);
	auto need_height = caret.y - push_caret.y;
	caret = push_caret;
	clipping = push_clip;
	return need_height;
}

static void paint_effect(variants effect, int height, int mode) {
	auto need_height = get_height(effect);
	auto push_caret = caret;
	caret.y += (height - need_height) / 2;
	paint_effect(effect, push_caret, mode);
}

void playercardi::paint_statistic() const {
	auto push_caret = caret;
	auto push_stroke = fore_stroke;
	auto push_font = font;
	auto push_width = width; width = 320;
	font = metrics::h2;
	caret.x += 160;
	// Name
	caret.y = push_caret.y + 25;
	fore_stroke = colors::active;
	textvalue(getnm(id));
	// Level
	caret.y = push_caret.y + 60;
	if(level == 0)
		textvalueng("X", 0);
	else
		textvalueng(level);
	// Initiative
	caret.y = push_caret.y + 232;
	font = metrics::h1;
	textvalue(initiative);
	// Upper
	caret.y = push_caret.y + 80;
	paint_effect(upper, 120, 0);
	// Lower
	caret.y = push_caret.y + 250;
	paint_effect(lower, 120, 1);
	//
	width = push_width;
	font = push_font;
	fore_stroke = push_stroke;
	caret = push_caret;
}

playercardi* playerdeck::choose(const char* title, bool need_remove, fnevent proc) {
	auto v = (int)deck::choose(title, bsdata<playercardi>::source, need_remove);
	return bsdata<playercardi>::elements + v;
}

void playercardi::paint() const {
	if(!owner)
		return;
	paint_ability_card(owner->fore);
	paint_statistic();
}

static void active_hexagon() {
	auto push_fore = fore;
	auto push_fsize = fsize;
	fore = colors::active;
	fsize = size - 1;
	hexagon();
	fsize = size - 2;
	hexagon();
	fore = push_fore;
	fsize = push_fsize;
}

static void hexagon(color v) {
	auto push_fore = fore;
	auto push_fsize = fsize;
	fore = v;
	fsize = size;
	hexagon();
	fore = push_fore;
	fsize = push_fsize;
}

static void hitpoints(int value) {
	auto push_caret = caret;
	auto push_font = font;
	caret.y += 2 * size / 3;
	font = metrics::h2;
	textvalue(value);
	caret = push_caret;
	font = push_font;
}

void creaturei::paint() const {
	if(isplayer())
		hexagon(colors::green);
	else if(is(Elite))
		hexagon(colors::yellow);
	else
		hexagon(colors::white);
	hitpoints(gethp());
	if(active == this)
		active_hexagon();
}

static void object_afterpaint(const object* ps) {
	if(bsdata<creaturei>::have(ps->data))
		((creaturei*)ps->data)->paint();
}

static void tips() {
	if(!hilite_object)
		return;
	auto push_caret = caret;
	caret.x = getwidth() - 320 * 2 - metrics::padding * 2 - metrics::border * 2;
	caret.y = 35;
	auto i = bsdata<playercardi>::source.indexof(hilite_object);
	if(i != -1)
		((playercardi*)bsdata<playercardi>::source.ptr(i))->paint();
	caret = push_caret;
}

void ui_initialize() {
	draw::ptips = tips;
	draw::object::afterpaint = object_afterpaint;
}