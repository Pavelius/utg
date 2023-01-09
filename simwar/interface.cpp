#include "calendar.h"
#include "collection.h"
#include "crt.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "game.h"
#include "player.h"
#include "unit.h"
#include "widget.h"

using namespace draw;

const int button_height = 20;
fnevent input_province;
void log_text(const char* format, ...);

static color player_colors[] = {
	{40, 40, 40},
	{97, 189, 79},
	{242, 214, 0},
	{255, 159, 26},
	{235, 90, 70},
	{195, 119, 224},
	{0, 121, 191},
};

static bool troops_in_province(const void* object) {
	return ((troop*)object)->province == province;
}

static bool building_in_province(const void* object) {
	return ((building*)object)->province == province;
}

static bool troops_player(const void* object) {
	return ((troop*)object)->player == player;
}

static color getplayercolor(playeri* p) {
	if(!p)
		return player_colors[0];
	return player_colors[p - bsdata<playeri>::elements + 1];
}

static void textcn(const char* format) {
	auto push_caret = caret;
	caret.x -= (textw(format) + 1) / 2;
	caret.y -= (texth() + 1) / 2;
	text(format);
	caret = push_caret;
}

static void textcn(const char* format, unsigned flags) {
	auto push_caret = caret;
	caret.x -= (textw(format) + 1) / 2;
	caret.y -= (texth() + 1) / 2;
	text(format, -1, flags);
	caret = push_caret;
}

static void textcw(const char* format) {
	auto push_caret_x = caret.x;
	caret.x -= (textw(format) + 1) / 2;
	text(format);
	caret.x = push_caret_x;
}

static void buttonback(int size, void* data) {
	auto push_alpha = alpha;
	rectpush push;
	width = size * 2;
	height = button_height - 1;
	caret.y -= (height + 1) / 2;
	caret.x -= (width + 1) / 2;
	alpha = (64 * push_alpha) >> 8;
	rectf();
	alpha = push_alpha;
	rectb();
	if(data && ishilite()) {
		hilite_object = data;
		if(hot.key == MouseLeft && !hot.pressed)
			execute(buttonparam, (long)data);
	}
}

static void paint_vborder() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	line(caret.x, caret.y + texth());
	fore = push_fore;
	caret = push_caret;
	caret.x += metrics::padding + 1;
}

static void hiliting_object(const void* object, int minimal_width) {
	rectpush push;
	height = texth();
	width = minimal_width;
	ishilite(object);
}

static void field(const char* format, int minimal_width) {
	caret.x += metrics::padding * 2;
	auto push_width = width;
	auto push_height = height;
	auto push_caret = caret;
	textfs(format);
	caret.x += (minimal_width - width) / 2;
	textf(format);
	caret = push_caret;
	caret.x += minimal_width + metrics::padding * 4;
	width = push_width;
	height = push_height;
}

static void field(cost_s v, const char* format, int width, int v1, int v2, int v3) {
	auto& e = bsdata<costi>::elements[v];
	if(!width)
		width = 60;
	char temp[260]; stringbuilder sb(temp);
	if(format)
		sb.add(format, v1, v2, v3);
	else {
		sb.add("%1i", v1);
		if(v2)
			sb.add("/%1i", v2);
		if(v3)
			sb.add("%+1i", v3);
	}
	hiliting_object(&e, width);
	field(temp, width);
	paint_vborder();
}

static void field(cost_s v, const char* format, int width, const costa& a1, const costa& a2, const costa& a3) {
	field(v, format, width, a1[v], a2[v], a3[v]);
}

static void field_date() {
	char temp[260]; stringbuilder sb(temp);
	game.getname(sb);
	field(temp, 120);
	paint_vborder();
}

static void paint_cost(const costa& v, const costa& u, const costa& n) {
	field_date();
	field(Gold, 0, 100, v, n, u);
	field(Mana, 0, 80, v, n, u);
	field(Happiness, "%3i", 40, v, n, u);
	field(Fame, "%1i", 0, v, n, u);
	field(Warfire, "%1i/%3i", 0, v, n, u);
	field(Lore, 0, 120, v, n, u);
	field(Trade, 0, 80, v, n, u);
}

void status_info() {
	auto push_caret = caret;
	caret.y += metrics::padding;
	paint_cost(player->resources, player->income, player->upgrade);
	caret = push_caret;
	caret.y += texth() + metrics::padding * 2;
}

static void hot_keys() {
	switch(hot.key) {
	case MouseRight:
		if(hot.pressed) {
			auto pt = camera + hot.mouse;
			log_text("Province position(%1i %2i) landscape(Plains)", pt.x, pt.y);
		}
		break;
	}
}

static void background_map() {
	auto pi = gres("silentseas", "maps");
	image(pi, 0, 0);
	hot_keys();
}

static void texth1(const char* format) {
	auto push_font = font;
	auto push_fore = fore;
	fore = colors::window;
	font = metrics::h1;
	textcn(format);
	fore = push_fore;
	font = push_font;
}

static void texth2(const char* format) {
	auto push_font = font;
	auto push_fore = fore;
	fore = colors::window;
	font = metrics::h2;
	textcn(format);
	fore = push_fore;
	font = push_font;
}

static void stroke_texth2(const char* format) {
	rectpush push;
	auto push_stroke = fore_stroke;
	auto push_font = font;
	auto push_fore = fore;
	fore_stroke = colors::white;
	fore = colors::window;
	font = metrics::h2;
	caret.x -= (textw(format) + 1) / 2;
	caret.y -= (texth() + 1) / 2;
	text(format, -1, TextStroke);
	fore_stroke = push_stroke;
	fore = push_fore;
	font = push_font;
}

static void paint_shield(int index, bool need_stroke = false) {
	auto p = gres("shields", "art/sprites");
	if(need_stroke)
		stroke(caret.x, caret.y, p, index, 0, 2);
	else
		image(p, index, 0);
}

static void border_circle(int size) {
	auto push_fore = fore;
	fore = colors::border;
	circle(size);
	fore = push_fore;
}

void provincei::paint() const {
	if(owner)
		paint_shield(owner->shield);
	if(input_province) {
		if(ishilite(16, this)) {
			paint_shield(player->shield, true);
			hot.cursor = cursor::Hand;
			if(hot.key == MouseLeft && !hot.pressed)
				execute(input_province, (int)this);
		}
	}
}

void troop::paint() const {
	auto push_color = fore;
	fore = getplayercolor(player);
	buttonback(43, 0);
	fore = colors::window;
	textcn(type->getname());
	fore = push_color;
}

static int compare_troop(const void* v1, const void* v2) {
	auto p1 = *((troop**)v1);
	auto p2 = *((troop**)v2);
	auto e1 = p1->type->effect[Level];
	auto e2 = p2->type->effect[Level];
	if(e1 != e2)
		return e1 - e2;
	return (int)p1->type - (int)p2->type;
}

static void add_widget(const char* id, unsigned char priority, bool absolute_position = true) {
	auto pm = bsdata<widget>::find(id);
	if(!pm)
		return;
	auto p = addobject(0, 0);
	p->data = pm;
	p->priority = priority;
	if(absolute_position)
		p->set(drawable::AbsolutePosition);
}

static void object_painting(const object* p) {
	if(bsdata<widget>::have(p->data))
		((widget*)p->data)->paint();
	else if(bsdata<provincei>::have(p->data))
		((provincei*)p->data)->paint();
	else if(bsdata<troop>::have(p->data))
		((troop*)p->data)->paint();
}

void update_provinces() {
	for(auto& e : bsdata<provincei>()) {
		auto p = addobject(e.position.x, e.position.y);
		p->data = &e;
		p->priority = 1;
	}
}

void ui_initialize() {
	object::painting = object_painting;
	object::initialize();
	widget::add("BackgroundMap", background_map);
	add_widget("BackgroundMap", 0, false);
}
