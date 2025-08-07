#include "draw.h"
#include "draw_board.h"
#include "draw_object.h"

using namespace draw;

static void indicator(const char* id) {
}

static void paint_abilitites() {
	pushrect push;
	board_tool_panel(48, 40);
	board_tool_button("Food", "12");
	board_tool_button("Clue", "13");
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