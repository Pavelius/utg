#include "answers.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "main.h"

using namespace draw;

static void paint_sprite() {
	image((sprite*)last_object->data, 0, 0);
}

static void add_worldmap() {
	addobject({0, 0}, gres("worldmap", "art/objects"), paint_sprite, 1);
}

static void add_monsters() {
	//auto p = addobject(764, 128);
	//p->data = gres("byakhee", "art/objects");
	//p->priority = 81;
}

static void add_gates() {
	//auto p = addobject(764, 108);
	//p->data = gres("gates", "art/objects");
	//p->frame = 1;
	//p->priority = 11;
}

static void add_characters() {
	addobject(game.location->position, gres("characters", "art/objects"), paint_sprite, 11);
}

static void add_clues() {
}

void update_board() {
	clear_objects();
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

static void objects_paint(const object* pointer) {
	auto pd = pointer->data;
	if(&game==pd)
		image(gres("characters", "art/objects"), game.investigator_index, 0);
	else
		image((sprite*)pd, pointer->frame, 0);
}

static void main_background() {
	strategy_background();
	paint_objects();
}

static void main_finish() {
	input_camera();
}

void initialize_ui() {
	pbackground = main_background;
	pfinish = main_finish;
}