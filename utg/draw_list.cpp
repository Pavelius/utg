#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"
#include "io_stream.h"

using namespace draw;

static int	list_maximum, list_perpage;
static int* list_current;
datasource	list_data;

sprite* metrics::list = (sprite*)loadb("art/tool/tree.pma");

void draw::showactive() {
	auto push_fore = fore;
	fore = colors::active;
	rectf();
	fore = push_fore;
}

void draw::partoffset(int x, int y) {
	caret.x += x; caret.y += y;
	width -= x; height -= y;
}

void draw::showborder() {
	auto push_fore = fore;
	fore = colors::border;
	rectb();
	fore = push_fore;
}

void draw::showbackground() {
	pushrect push;
	auto push_fore = fore;
	fore = colors::window;
	width -= 1; height -= 1;
	rectf();
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
	switch(hkey) {
	case MouseWheelUp:
		if(gui.hilighted && origin)
			execute(cbsetint, origin - 1, 0, &origin);
		break;
	case MouseWheelDown:
		if(gui.hilighted && origin + perpage < list_maximum - 1)
			execute(cbsetint, origin + 1, 0, &origin);
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
	auto push_clip = clipping;
	setclipall();
	gui.hilighted = ishilite();
	list_maximum = list_data.count;
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
		if(ishilite() && hkey == MouseLeft && hpressed)
			execute(cbsetint, gui.index, 0, &current);
		auto push_value = gui.value;
		gui.data = list_data.ptr(gui.index);
		auto push_caret = caret;
		if(gui.pgetname) {
			sb.clear();
			gui.value = gui.pgetname(gui.data, sb);
		}
		prow();
		caret = push_caret;
		gui.value = push_value;
		caret.y += height;
	}
	height = push_height;
	clipping = push_clip;
}

static void table_text() {
	textc(gui.value);
}

static void table_icon() {
	auto push_caret = caret;
	caret.y += height / 2;
	caret.x += height / 2;
	image(metrics::list, gui.number, 0);
	caret = push_caret;
}

void table_text_icon() {
	auto push_width = width;
	table_icon();
	caret.x += height; width -= height;
	table_text();
	width = push_width;
}