#include "draw.h"
#include "main.h"

using namespace draw;

void status_info(void) {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x += 4;
	caret.y += 4;
	fore = push_fore;
	caret = push_caret;
	caret.y += 4 * 4 + 24;
}