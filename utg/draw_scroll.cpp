#include "draw.h"
#include "draw_scroll.h"
#include "math.h"

using namespace draw;

int scroll_size = 16;

void correct_list(int& origin, int maximum, int per_page) {
	if(origin + per_page > maximum)
		origin = maximum - per_page + 1;
	if(origin < 0)
		origin = 0;
}

static void clip_mouse_input() {
	if(ishilite()) {
		switch(hkey) {
		case MouseLeft: case MouseRight:
		case MouseLeftDBL: case MouseMove:
			hkey = InputUpdate;
			break;
		}
	}
}

static void input_slider_area(int& origin, int maximum, int row_height, int per_page) {
	static int bar_position_start;
	if(!maximum)
		return;
	auto slider_height = height;
	auto bar_position = height * origin / (maximum + 1);
	auto bar_height = height * per_page / (maximum + 1) + 1;
	pushrect push;
	rect ru = {caret.x, caret.y, caret.x + width, caret.y + bar_position - 1};
	rect rb = {caret.x, caret.y + bar_position, caret.x + width, caret.y + bar_position + bar_height - 1};
	rect rd = {caret.x, caret.y + bar_position + bar_height, caret.x + width, caret.y + slider_height};
	if(dragactive(&origin)) {
		if(hkey == MouseMove) {
			auto new_origin = (bar_position_start + (hmouse.y - dragmouse.y)) * maximum / slider_height;
			execute(cbsetint, new_origin, 0, &origin);
		}
	} else {
		if(hkey == MouseLeft && hmouse.in(ru)) {
			if(!hpressed)
				execute(cbsetint, origin - per_page + 1, 0, &origin);
		} else if(hkey == MouseLeft && hmouse.in(rd)) {
			if(!hpressed)
				execute(cbsetint, origin + per_page - 1, 0, &origin);
		} else if(hmouse.in(rb)) {
			if(hkey == MouseLeft && hpressed) {
				dragbegin(&origin, 0);
				bar_position_start = bar_position;
			}
		}
		clip_mouse_input();
	}
}

static void paint_slider(bool vertical) {
	auto f1 = colors::button.mix((ishilite() && hpressed) ? colors::border : colors::window);
	auto f2 = colors::button;
	if(vertical)
		gradh(f1, f2);
	else
		gradv(f2, f1);
	rectb();
}

static void paint_scroll_area(int& origin, int maximum, int row_height) {
	if(!maximum)
		return;
	auto per_page = (height + row_height - 1) / row_height;
	if(per_page >= maximum)
		return;
	correct_list(origin, maximum, per_page);
	auto bar_position = height * origin / (maximum + 1);
	auto bar_height = height * per_page / (maximum + 1) + 1;
	pushrect push;
	pushfore push_fore(colors::button);
	// Paint slider
	auto push_alpha = alpha; alpha = 64;
	rectf();
	alpha = push_alpha;
	// Paint slider
	caret.y += bar_position;
	height = bar_height;
	paint_slider(true);
}

static void paint_scroll_slider(int& origin, int maximum, int row_height) {
	if(!maximum)
		return;
	auto per_page = (height + row_height - 1) / row_height;
	if(per_page >= maximum)
		return;
	auto bar_position = height * origin / (maximum + 1);
	auto bar_height = height * per_page / (maximum + 1) + 1;
	pushrect push;
	pushfore push_fore(colors::button);
	auto push_alpha = alpha; alpha = 192;
	caret.y += bar_position;
	height = bar_height;
	rectf();
	alpha = push_alpha;
}

void paint_scroll(int& origin, int maximum, int row_height) {
	pushrect push;
	caret.x = caret.x + width - scroll_size; width = scroll_size;
	if(hmouse.in(getrect()) || dragactive(&origin))
		paint_scroll_area(origin, maximum, row_height);
	else {
		caret.x = push.caret.x + push.width - 2; width = 2 * 2;
		paint_scroll_slider(origin, maximum, row_height);
	}
}

void input_scroll(int& origin, int maximum, int row_height) {
	auto per_page = (height + row_height - 1) / row_height;
	if(per_page >= maximum)
		return;
	correct_list(origin, maximum, per_page);
	if(ishilite()) {
		switch(hkey) {
		case MouseWheelUp: execute(cbsetint, origin - 1, 0, &origin); break;
		case MouseWheelDown: execute(cbsetint, origin + 1, 0, &origin); break;
		}
	}
	pushrect push;
	caret.x = caret.x + width - scroll_size; width = scroll_size;
	input_slider_area(origin, maximum, row_height, per_page);
}

static void paint_scroll_area_horiz(int& origin, int maximum, int row_height) {
	if(!maximum)
		return;
	auto per_page = (width + row_height - 1) / row_height;
	if(per_page >= maximum)
		return;
	correct_list(origin, maximum, per_page);
	auto bar_position = width * origin / (maximum + 1);
	auto bar_height = width * per_page / (maximum + 1) + 1;
	pushrect push;
	pushfore push_fore(colors::button);
	// Paint slider
	auto push_alpha = alpha; alpha = 64;
	rectf();
	alpha = push_alpha;
	// Paint slider
	caret.x += bar_position;
	width = bar_height;
	paint_slider(false);
}

static void paint_scroll_slider_horiz(int& origin, int maximum, int row_height) {
	if(!maximum)
		return;
	auto per_page = (width + row_height - 1) / row_height;
	if(per_page >= maximum)
		return;
	auto bar_position = width * origin / (maximum + 1);
	auto bar_height = width * per_page / (maximum + 1) + 1;
	pushrect push;
	pushfore push_fore(colors::button);
	auto push_alpha = alpha; alpha = 192;
	caret.x += bar_position; width = bar_height;
	rectf();
	alpha = push_alpha;
}

void paint_scroll_horiz(int& origin, int maximum, int row_height) {
	pushrect push;
	caret.y = caret.y + height - scroll_size; height = scroll_size;
	if(hmouse.in(getrect()) || dragactive(&origin))
		paint_scroll_area_horiz(origin, maximum, row_height);
	else {
		caret.y = push.caret.y + push.height - 2; height = 2 * 2;
		paint_scroll_slider_horiz(origin, maximum, row_height);
	}
}

static void input_slider_area_horiz(int& origin, int maximum, int row_height, int per_page) {
	static int bar_position_start;
	if(!maximum)
		return;
	auto slider_height = width;
	auto bar_position = width * origin / (maximum + 1);
	auto bar_height = width * per_page / (maximum + 1) + 1;
	pushrect push;
	rect ru = {caret.x, caret.y, caret.x + width, caret.y + bar_position - 1};
	rect rb = {caret.x + bar_position, caret.y, caret.x + bar_position + bar_height - 1, caret.y + height};
	rect rd = {caret.x + bar_position + bar_height, caret.y, caret.x + slider_height, caret.y + height};
	if(dragactive(&origin)) {
		if(hkey == MouseMove) {
			auto new_origin = (bar_position_start + (hmouse.x - dragmouse.x)) * maximum / slider_height;
			execute(cbsetint, new_origin, 0, &origin);
		}
	} else {
		if(hkey == MouseLeft && hmouse.in(ru)) {
			if(!hpressed)
				execute(cbsetint, origin - per_page + 1, 0, &origin);
		} else if(hkey == MouseLeft && hmouse.in(rd)) {
			if(!hpressed)
				execute(cbsetint, origin + per_page - 1, 0, &origin);
		} else if(hmouse.in(rb)) {
			if(hkey == MouseLeft && hpressed) {
				dragbegin(&origin, 0);
				bar_position_start = bar_position;
			}
		}
		clip_mouse_input();
	}
}

void input_scroll_horiz(int& origin, int maximum, int row_height) {
	auto per_page = (width + row_height - 1) / row_height;
	if(per_page >= maximum)
		return;
	correct_list(origin, maximum, per_page);
	pushrect push;
	caret.y = caret.y + height - scroll_size; height = scroll_size;
	input_slider_area_horiz(origin, maximum, row_height, per_page);
}