#include "draw.h"
#include "draw_strategy.h"
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

static void paint_tile() {
	auto p = (tilei*)last_object->data;
	auto ps = gres(p->id, "art/tiles");
	image(ps, 0, 0);
}

static void paint_decoration() {
	auto p = (decoration*)last_object->data;
	auto ps = gres(p->parent->id, "art/tiles");
	image(ps, 0, 0);
}

static object* add_tile(tilei* ps, point position, bool inverse) {
	auto pt = h2p(position) - ps->offset;
	return addobject(pt, (void*)ps, paint_tile, 0, ps->priority, 255, inverse ? (ImageMirrorH | ImageMirrorV) : 0);
}

void scenariotilei::updateui() const {
	auto p = findobject(this);
	if(p)
		return;
	auto ps = bsdata<tilei>::find(type);
	if(ps) {
		if(ps->isdecoration()) {
			auto pd = decoration::add(type, position);
			pd->updateui();
		} else {
			creating_tile(ps, position, inverse);
			add_tile(ps, position, inverse);
		}
	}
	auto pm = bsdata<monsteri>::find(type);
	if(pm) {
		auto p = creaturei::add(type, position, false);
		p->set(Hostile);
		p->updateui();
	}
}

void creaturei::updateui() const {
	auto p = findobject(this);
	if(!p) {
		auto pt = h2p(getposition());
		focusing(pt);
		addobject(pt, (void*)this, ftpaint<creaturei>, 4);
		//p->resource = draw::getres(getid(), "art/creatures");
		splash_screen(500);
	}
}

void decoration::updateui() const {
	auto p = findobject(this);
	if(!p) {
		auto pt = h2p(getposition());
		focusing(pt);
		addobject(pt, (void*)this, paint_decoration, 0, 14);
		//p->resource = draw::getres(parent->id, "art/tiles");
	}
}

void gamei::focusing(point pt) {
	::focusing(h2p(pt));
}

static draworder* last_order;

void ordermove(void* object, point hp, int time, bool depended) {
	auto p = findobject(object);
	if(!p)
		return;
	auto pt = h2p(hp);
	auto po = p->addorder(time, depended ? last_order : 0);
	po->position = pt;
	last_order = po;
}

static void paint_string() {
}

static draworder* floatstring(point pt, color fc, const char* format) {
	auto pb = addobject(pt, (void*)format, paint_string, 101);
	//pb->string = szdup(format);
	//pb->font = metrics::h1;
	//pb->fore = fc;
	pb->alpha = 0;
	pb->priority = 101;
	auto po = pb->addorder(500);
	po->position.y -= size;
	po->start.alpha = 255;
	po->alpha = 255;
	po->autoclean();
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
	auto dx = p2->position.x - p1->position.x;
	auto dy = p2->position.y - p1->position.y;
	auto dz = isqrt(dx * dx + dy * dy);
	if(!dz)
		return;
	draworder* po;
	auto push_position = *p1;
	// Move enemy
	po = p1->addorder(150);
	po->position.x += calculate(size / 3, dx, dz);
	po->position.y += calculate(size / 3, dy, dz);
	po->wait();
	// Move back from enemy
	po = p1->addorder(250);
	po->position.x = push_position.position.x;
	po->position.y = push_position.position.y;
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

void indexable::disappear() const {
	auto p1 = findobject(this);
	if(!p1)
		return;
	auto po = p1->addorder(350);
	po->alpha = 0;
	po->autoclean();
}

void indexable::fixmove(point hex) const {
	auto p1 = findobject(this);
	if(!p1)
		return;
	auto pt = h2p(hex); focusing(pt);
	auto po = p1->addorder(400);
	po->position = pt;
	po->wait();
}

void indexable::fixexperience(int value) const {
	fixvalue(getposition(), value, colors::blue);
}

void indexable::fixgood(const char* format, ...) const {
	char temp[260]; stringbuilder sb(temp);
	sb.addv(format, xva_start(format));
	floatstring(h2p(getposition()), colors::text, temp);
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
	//auto push_event = object::afterpaintall;
	//object::afterpaintall = paint_moverange;
	splash_screen(500);
	auto result = an.choose();
	//object::afterpaintall = push_event;
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
	//auto push_event = object::afterpaintall;
	//object::afterpaintall = paint_targets;
	splash_screen(500);
	auto result = (creaturei*)an.choose(title, getnm("Cancel"), 1);
	//object::afterpaintall = push_event;
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
	fsize = size - 2;
	hexagon();
	fsize = size - 3;
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

static void textvalue(int value, int dx, int dy, color v) {
	if(value <= 0)
		return;
	auto push_caret = caret;
	auto push_font = font;
	auto push_fore = fore;
	fore = v;
	caret.y += dy;
	caret.x += dx;
	font = metrics::h2;
	textvalue(value);
	fore = push_fore;
	caret = push_caret;
	font = push_font;
}

static void paint_actives() {
	auto push_width = width;
	width = 320;
	for(auto& e : bsdata<activecardi>()) {
		if(!e)
			continue;
		answers::paintcell(-1, &e, getnm(e.card->id), 0);
	}
	width = push_width;
}

static void paint_elements() {
	for(auto i = Fire; i <= Dark; i = (element_s)(i + 1)) {
		circle(14);
		if(game.elements[i]) {
			auto push_alpha = alpha;
			if(game.elements[i] == 1)
				alpha = 96;
			else
				alpha = 255;
			image(gres("elements32"), i, 0);
			alpha = push_alpha;
		}
		caret.x += 32 + 3;
	}
}

void status_info(void) {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x += 17;
	caret.y += 17;
	paint_elements();
	fore = push_fore;
	caret = push_caret;
	caret.y += 36;
}

void creaturei::paint() const {
	const char* id = 0;
	auto pm = getmonster();
	if(pm)
		id = pm->id;
	auto pp = getplayer();
	if(pp)
		id = pp->id;
	auto ps = gres(id, "art/creatures");
	image(ps, 0, 0);
	if(isplayer())
		hexagon(colors::green);
	else if(is(Elite))
		hexagon(colors::yellow);
	else
		hexagon(colors::white);
	textvalue(gethp(), 0, 2 * size / 3, colors::red);
	textvalue(getexperience(), -2 * size / 3, -size / 3, colors::blue);
	textvalue(getcoins(), 2 * size / 3, -size / 3, colors::yellow);
	if(active == this)
		active_hexagon();
}

static void tips() {
	if(!hilite_object)
		return;
	auto push_caret = caret;
	caret.x = getwidth() - 320 * 2 - metrics::padding * 2 - metrics::border * 2;
	caret.y = 39;
	auto i = bsdata<playercardi>::source.indexof(hilite_object);
	if(i != -1)
		((playercardi*)bsdata<playercardi>::source.ptr(i))->paint();
	caret = push_caret;
}

static void overlaped_window() {
	auto push_caret = caret;
	caret.y = 37 + metrics::padding + metrics::border;
	caret.x = metrics::padding + metrics::border;
	paint_actives();
	caret = push_caret;
}

static void main_background() {
	strategy_background();
	paint_objects();
}

static void main_finish() {
	input_camera();
}

void ui_initialize() {
	draw::ptips = tips;
	draw::pbackground = main_background;
	draw::pfinish = main_finish;
	//draw::object::afterpaint = object_afterpaint;
	//draw::object::afterpaintall = overlaped_window;
}