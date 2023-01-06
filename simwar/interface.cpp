#include "crt.h"
#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "game.h"
#include "player.h"
#include "widget.h"

using namespace draw;

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

static void paint_cost(const costable& v, const costable& u, const costable& n) {
	char temp[260]; stringbuilder sb(temp);
	for(auto i = 0; i <= Warfire; i++) {
		auto& e = bsdata<costi>::elements[i];
		sb.clear();
		if(v.cost[i])
			sb.add("%1i", v.cost[i]);
		if(n.cost[i])
			sb.add("/%1i", n.cost[i]);
		if(u.cost[i])
			sb.add("%+1i", u.cost[i]);
		if(!temp[0])
			continue;
		auto minimal_width = 60;
		if(e.width)
			minimal_width = e.width;
		hiliting_object(&e, minimal_width);
		field(temp, minimal_width);
		paint_vborder();
	}
}

void status_info() {
	auto push_caret = caret;
	paint_cost(*player, player->upkeep, player->upgrade);
	caret = push_caret;
	caret.y += texth() + metrics::padding;
}

static void background_map() {
	auto pi = gres("silentseas", "maps");
	image(pi, 0, 0);
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
}

void ui_initialize() {
	object::painting = object_painting;
	object::initialize();
	widget::add("BackgroundMap", background_map);
	add_widget("BackgroundMap", 0, false);
}
