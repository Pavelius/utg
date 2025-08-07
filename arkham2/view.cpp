#include "ability.h"
#include "bsdata.h"
#include "draw.h"
#include "draw_board.h"
#include "draw_object.h"

using namespace draw;

static void paint_value(abilityn v) {
	auto p = bsdata<abilityi>::elements + v;
	board_tool_button(p->id, p, "12");
}

static void paint_abilitites() {
	pushrect push;
	board_tool_panel(40);
	width = 128;
	paint_value(Speed);
	width = 64;
	paint_value(Speed);
	paint_value(Sneak);
	paint_value(Fight);
	paint_value(Will);
	paint_value(Lore);
	paint_value(Luck);
}

static void background() {
	fillwindow();
	paint_abilitites(); caret.y += 40; height -= 40;
}

static void main_scene() {
	background();
	paint_objects();
	input_camera();
}

void main_menu() {
	scene(main_scene);
}