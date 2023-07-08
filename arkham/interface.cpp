#include "answers.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "main.h"

using namespace draw;

static void add_worldmap() {
	auto p = addobject(0, 0);
	//p->resource = getbackground("worldmap");
	p->priority = 1;
}

static void add_monsters() {
	auto p = addobject(764, 128);
	//p->resource = getres("byakhee");
	p->priority = 90;
}

static void add_gates() {
	auto p = addobject(764, 108);
	//p->resource = getres("gates");
	p->frame = 1;
	p->priority = 10;
}

static void add_clues() {
}

void update_board() {
	clearobjects();
	add_worldmap();
	add_monsters();
	add_gates();
	add_clues();
}

static void add_label(const char* label) {
	if(!label)
		return;
	auto push_fore = fore;
	fore = colors::h3;
	text(label);
	caret.x += textw(label) + 2;
	fore = push_fore;
}

static void add_info(const char* label, const char* value) {
	add_label(label);
	text(value);
	caret.x += textw(value) + textw(' ') * 2;
}

static void add_info(const char* label, int value) {
	char temp[128]; stringbuilder sb(temp);
	sb.add("%1i", value);
	add_info(label, temp);
}

void status_info() {
	auto push_caret = caret;
	caret.y += metrics::border;
	caret.x += textw(' ');
	for(auto& e : bsdata<abilityi>()) {
		auto v = ability_s(&e - bsdata<abilityi>::elements);
		if(e.is(abilityi::Stat) || e.is(abilityi::Indicator))
			add_info(getnm(e.id), game.get(v));
	}
	caret = push_caret;
	caret.y += texth() + metrics::border*2 + metrics::padding;
}

static void main_background() {
	strategy_background();
	paint_objects();
}

static void main_finish() {
	inputcamera();
}

void initialize_ui() {
	pbackground = main_background;
	pfinish = main_finish;
}