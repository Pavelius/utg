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

static bool show_names;

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
	if(e.frame != -1)
		sb.add(":%1i:", e.frame);
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

static void field(cost_s v, const char* format, int width, const costa& a1, const costa& a2, const costac& a3) {
	field(v, format, width, a1[v], a2[v], a3[v]);
}

static void field_date() {
	char temp[260]; stringbuilder sb(temp);
	game.getname(sb);
	field(temp, 120);
	paint_vborder();
}

static void paint_cost(const costa& v, const costac& u, const costa& n) {
	field_date();
	field(Gold, 0, 100, v, n, u);
	field(Mana, 0, 80, v, n, u);
	field(Happiness, "%3i", 40, v, n, u);
	field(Fame, "%1i", 0, v, n, u);
	field(Warfire, "%3i", 0, v, n, u);
	field(Lore, 0, 120, v, n, u);
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
	case Ctrl + 'N':
		show_names = !show_names;
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

static void paint_hilite_province(int index, bool need_stroke = false) {
	auto push_fore = fore;
	auto push_alpha = alpha;
	fore = colors::red;
	alpha = 32;
	circlef(32);
	alpha = push_alpha;
	fore = push_fore;
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

static void paint_neighbor() {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::red;
	neightbors source;
	source.selectn(province);
	for(auto p : source) {
		if(!p->isvisible())
			continue;
		caret = push_caret;
		point pt = p->position - camera;
		line(pt.x, pt.y);
	}
	fore = push_fore;
	caret = push_caret;
}

void add_line_upkeep(const provincei* province, stringbuilder& sb);

static void paint_income(const provincei* province) {
	char temp[260]; stringbuilder sb(temp); temp[0] = 0;
	add_line_upkeep(province, sb);
	if(temp[0] == 0)
		return;
	rectpush push;
	auto push_fore = fore;
	textfs(temp);
	caret.x -= width / 2;
	caret.y -= texth() / 2;
	fore = colors::black;
	textf(temp);
	fore = push_fore;
}

void provincei::paint() const {
	if(player) {
		paint_hilite_province(player->shield);
		//paint_shield(player->shield);
	}
	if(show_names)
		stroke_texth2(getname());
	if(province == this)
		paint_neighbor();
	paint_income(this);
	if(input_province) {
		if(ishilite(24, this)) {
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

static void remove_object(array& source) {
	for(auto& e : bsdata<object>()) {
		if(source.indexof(e.data) != -1)
			e.clear();
	}
}

void update_provinces_ui() {
	remove_object(bsdata<provincei>::source);
	for(auto& e : bsdata<provincei>()) {
		if(!e.isvisible())
			continue;
		auto p = addobject(e.position.x, e.position.y);
		p->data = &e;
		p->priority = 1;
	}
}

static void ui_background() {
	strategy_background();
	paint_objects();
}

static void ui_finish() {
	inputcamera();
}

void ui_initialize() {
	object::painting = object_painting;
	object::initialize();
	widget::add("BackgroundMap", background_map);
	add_widget("BackgroundMap", 0, false);
	pbackground = ui_background;
	pfinish = ui_finish;
}
