#include "draw.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

static void panel(const char* format, int panel_width) {
	auto push_width = width;
	auto push_caret = caret;
	width = panel_width;
	texta(format, AlignCenterCenter);
	caret.x += width;
	auto push_fore = fore;
	fore = colors::border;
	line(caret.x, caret.y + height);
	caret.y = push_caret.y;
	caret.x += 1;
	fore = push_fore;
	width = push_width;
}

static const char* getmonsthname(int i) {
	static const char* names[] = {
		"January", "February",
		"March", "April", "May",
		"June", "July", "August",
		"September", "October", "November",
		"December"
	};
	return getnmof(maptbl(names, i - 1));
}

static void data_panel() {
	char temp[64]; stringbuilder sb(temp);
	sb.add("%1i %2 %3i", game.getmonthday(), getmonsthname(game.getmonth()), game.getyear());
	panel(temp, 120);
}

void status_info(void) {
	auto push_caret = caret;
	auto push_height = height;
	height = draw::texth() + metrics::padding * 2;
	data_panel();
	caret = push_caret;
	caret.y += height;
	height = push_height;
}

static void aftercreate_object(object* po) {
	if(bsdata<planeti>::have(po->data)) {
		auto p = (planeti*)po->data;
		po->shape = figure::CircleFill;
		po->size = xrand(5, 10);
		po->fore = colors::green;
		//po->set(object::Hilite);
	} else if(bsdata<systemi>::have(po->data)) {
		po->shape = figure::CircleFill;
		po->size = 32;
		po->fore = colors::yellow;
	}
}

static void afterpaint_object(const object* po) {
}

void initialize_interface() {
	object::aftercreate = aftercreate_object;
}