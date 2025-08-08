#include "ability.h"
#include "bsdata.h"
#include "draw.h"
#include "draw_board.h"
#include "draw_button.h"
#include "draw_object.h"
#include "player.h"

using namespace draw;

static void paint_value(abilityn v) {
	auto p = bsdata<abilityi>::elements + v;
	board_tool(p->id, p, player.abilitites[v], "%1i");
}

static void paint_indicator(abilityn v) {
	auto p = bsdata<abilityi>::elements + v;
	char temp[32]; stringbuilder sb(temp);
	sb.add("%1i/%2i", player.abilitites[v], player.abilitites[v]);
	board_tool(p->id, p, temp);
}

static void open_character() {
}

static void paint_abilitites() {
	pushrect push;
	board_tool_panel(40);
	width = 128;
	board_tool("EricColt", 'C'); fire(open_character); caret.x += width;
	width = 64;
	paint_value(Speed);
	paint_value(Sneak);
	paint_value(Fight);
	paint_value(Will);
	paint_value(Lore);
	paint_value(Luck);
	paint_indicator(Health);
	paint_indicator(Sanity);
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