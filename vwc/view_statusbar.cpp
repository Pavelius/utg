#include "draw.h"
#include "stringbuilder.h"
#include "view_statusbar.h"

using namespace draw;

static char status_text[260];
stringbuilder status_sb(status_text);
static rect status_rect;

void before_modal_statusbar() {
	status_sb.clear();
}

void status(const char* format, ...) {
	status_sb.addv(format, xva_start(format));
}

void tips_statusbar() {
	if(!status_sb)
		return;
	if(!status_rect)
		return;
	pushrect push;
	setpos(status_rect);
	textcnw(status_text);
}

void paint_statusbar() {
	auto push_height = height;
	auto dy = texth() + metrics::border * 2; height = dy;
	caret.y = getheight() - dy + 1;
	gradv(colors::form, colors::window, 0);
	strokeline();
	caret.y += metrics::border;
	status_rect.set(caret.x, caret.y, caret.x + width, caret.y + height);
	height = push_height;
	height -= dy;
}