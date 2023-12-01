#include "draw.h"
#include "pushvalue.h"

using namespace draw;

static surface	source;
static color	pallette[256];
static point	origin;
static unsigned char pixel_index;
static int		pixel_size = 4, pixel_index_hilite;

static void paint_hilite_index() {
	rectpush push;
	caret.x--;
	caret.y--;
	height += 2;
	width += 2;
	fore = fore.negative();
	rectb();
}

static void draw_pixel() {
	auto push_fore = fore;
	fore = pallette[pixel_index];
	rectf();
	if(ishilite()) {
		pixel_index_hilite = pixel_index;
		paint_hilite_index();
	}
	fore = push_fore;
	caret.x += width + 1;
}

static void control_pixels() {
	if(ishilite()) {
		switch(hot.key) {
		case MouseWheelUp:
			if(pixel_size < 64)
				execute(cbsetint, pixel_size + 1, 0, &pixel_size);
			break;
		case MouseWheelDown:
			if(pixel_size > 2)
				execute(cbsetint, pixel_size - 1, 0, &pixel_size);
			break;
		}
	}
}

void view_pixels() {
	control_pixels();
	rectpush push;
	pushvalue push_index(pixel_index);
	auto x2 = caret.x + push.width;
	auto y2 = caret.y + push.height;
	pixel_index_hilite = -1;
	auto n = origin;
	width = pixel_size; height = pixel_size;
	while(n.y < source.height && caret.y < y2) {
		n.x = origin.x;
		while(n.x < source.width && caret.x < x2) {
			pixel_index = *source.ptr(n.x, n.y);
			draw_pixel();
			n.x++;
		}
		caret.y += height + 1;
		caret.x = push.caret.x;
		n.y++;
	}
}

void initialize_pixels() {
	source.resize(32, 32, 8, true);
}