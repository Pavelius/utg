#include "crt.h"
#include "draw.h"
#include "pushvalue.h"
#include "view_statusbar.h"

using namespace draw;

static surface	source;
static color	pallette[256];
static point	origin, current_pixel;
point			pixel_position;
static unsigned char pixel_index;
static int		pixel_size = 4, pixel_index_hilite;

static void paint_hilite_index() {
	pushrect push;
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
		pixel_position = current_pixel;
		paint_hilite_index();
		status(getnm("PixelPosition"), pixel_position.x, pixel_position.y, pixel_index_hilite);
	}
	fore = push_fore;
	caret.x += width + 1;
}

static void control_pixels() {
	if(ishilite()) {
		switch(hkey) {
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

static void center_paint() {
	auto mx = (pixel_size + 1) * source.width;
	auto my = (pixel_size + 1) * source.height;
	if(mx < width)
		caret.x += (width - mx) / 2;
	if(my < height)
		caret.y += (height - my) / 2;
}

void view_pixels() {
	pushvalue push_clip(clipping);
	setclipall();
	center_paint();
	control_pixels();
	pushrect push;
	pushvalue push_index(pixel_index);
	auto x2 = caret.x + push.width;
	auto y2 = caret.y + push.height;
	pixel_index_hilite = -1;
	pixel_position.x = -1; pixel_position.y = -1;
	current_pixel = origin;
	width = pixel_size; height = pixel_size;
	while(current_pixel.y < source.height && caret.y < y2) {
		current_pixel.x = origin.x;
		while(current_pixel.x < source.width && caret.x < x2) {
			pixel_index = *source.ptr(current_pixel.x, current_pixel.y);
			draw_pixel();
			current_pixel.x++;
		}
		caret.y += height + 1;
		caret.x = push.caret.x;
		current_pixel.y++;
	}
}

void initialize_pixels() {
	source.resize(32, 32, 8, true);
}