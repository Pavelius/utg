#include "draw.h"
#include "main.h"

using namespace draw;

void status_info(void) {
	auto push_caret = caret;
	auto push_fore = fore;
	fore = colors::border;
	caret.x += 17;
	caret.y += 17;
	fore = push_fore;
	caret = push_caret;
	caret.y += 64;
}