#include "crt.h"
#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"

using namespace draw;

static int list_maximum, list_perpage;
static int* list_current;

sprite* metrics::list = (sprite*)loadb("art/tool/tree.pma");

void draw::showactive() {
	auto push_fore = fore;
	fore = colors::active;
	rectf();
	fore = push_fore;
}

void draw::showborder() {
	auto push_fore = fore;
	fore = colors::border;
	rectb();
	fore = push_fore;
}

static void list_input(int& current, int& origin, int perpage, int perline) {
	list_current = &current;
	if(current >= list_maximum)
		current = list_maximum - 1;
	if(current < 0)
		current = 0;
	if(current < origin)
		origin = current;
	if(origin + perpage < current)
		origin = current - perpage;
	switch(hot.key) {
	case MouseWheelUp:
		if(gui.hilighted && current)
			execute(cbsetint, current - 1, 0, &current);
		break;
	case MouseWheelDown:
		if(gui.hilighted && current < list_maximum - 1)
			execute(cbsetint, current + 1, 0, &current);
		break;
	case KeyUp:
		if(current)
			execute(cbsetint, current - 1, 0, &current);
		break;
	case KeyDown:
		if(current < list_maximum - 1)
			execute(cbsetint, current + 1, 0, &current);
		break;
	case KeyHome:
		if(current)
			execute(cbsetint, 0, 0, &current);
		break;
	case KeyEnd:
		if(current != list_maximum - 1)
			execute(cbsetint, list_maximum - 1, 0, &current);
		break;
	case KeyPageUp:
		if(current)
			execute(cbsetint, current - perpage, 0, &current);
		break;
	case KeyPageDown:
		execute(cbsetint, current + perpage, 0, &current);
		break;
	}
	if(list_maximum > origin + perpage + 1)
		list_maximum = origin + perpage + 1;
}

static void rectfhl(unsigned char alpha) {
	auto push_alpha = draw::alpha;
	draw::alpha = alpha;
	showactive();
	draw::alpha = push_alpha;
}

void draw::list(int& origin, int& current, int perline, fnevent prow) {
	if(!perline || !prow)
		return;
	list_perpage = height / perline;
	if(!list_perpage)
		return;
	list_maximum = gui.count;
	list_input(current, origin, list_perpage, perline);
	auto push_height = height;
	height = perline;
	char temp[1024]; stringbuilder sb(temp);
	for(gui.index = origin; gui.index < list_maximum; gui.index++) {
		gui.hilighted = ishilite();
		if(gui.index == current)
			rectfhl(128);
		else if(gui.hilighted)
			rectfhl(32);
		if(ishilite() && hot.key == MouseLeft && hot.pressed)
			execute(cbsetint, gui.index, 0, &current);
		gui.object = gui.ptr(gui.index);
		if(gui.pgetname) {
			sb.clear();
			gui.value = gui.pgetname(gui.object, sb);
		}
		if(true) {
			rectpush push;
			auto push_clip = clipping;
			setoffset(2, 2);
			setclip({caret.x, caret.y, caret.x + width, caret.y + height});
			prow();
			clipping = push_clip;
		}
		caret.y += height;
	}
	height = push_height;
}

static void table_text() {
	textc(gui.value);
}

static void table_icon() {
	auto push_caret = caret;
	caret.y += 16 / 2;
	caret.x += 16 / 2;
	image(metrics::list, gui.number, 0);
	caret = push_caret;
}

void table_text_icon() {
	table_icon();
	caret.x += 16;
	table_text();
}