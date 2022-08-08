#include "draw.h"
#include "draw_gui.h"
#include "draw_list.h"

using namespace draw;

static int list_maximum, list_perpage;
static int* list_current;

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
	case KeyUp:
	case MouseWheelUp:
		if(current)
			execute(cbsetint, current - 1, 0, &current);
		break;
	case MouseWheelDown:
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

static void hilighting() {
}

void draw::list(int& origin, int& current, int perline) {
	if(!perline)
		return;
	list_perpage = height / perline;
	if(!list_perpage)
		return;
	list_maximum = gui.count;
	list_input(current, origin, list_perpage, perline);
	auto push_height = height;
	height = perline;
	for(auto i = origin; i < list_maximum; i++) {
		auto push_fore = fore;
		if(i == current)
			hilighting();
		if(ishilite() && hot.key == MouseLeft && hot.pressed)
			execute(cbsetint, i, 0, &current);
		//if(prow)
		//	prow(gui.ptr(i));
		fore = push_fore;
		caret.y += height;
	}
	height = push_height;
}