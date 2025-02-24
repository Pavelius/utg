﻿#include "draw.h"
#include "io_stream.h"
#include "stringbuilder.h"

using namespace draw;

static char status_text[260];
static rect status_rect;
static stringbuilder status(status_text);

static surface bitdata(32, 32, 8);
static surface bitcopy(32, 32, 8);
static point bitdata_spot;
static int zoom = 16;

const int pallette_zoom = 14; // Pallette zoom view
static color pallette[256]; // Currently selected pallette
static int pallette_spot, pallette_current;

void set_light_theme();
void set_dark_theme();

static void rectb_current() {
	pushrect push_rect;
	pushfore push(colors::active.mix(colors::form));
	rectb();
	setoffset(1, 1);
	rectb();
}

static void rectb_spot() {
	pushfore push(colors::active);
	rectb();
}

static void paint_pallette() {
	pushrect push;
	pushfore push_fore;
	pallette_spot = -1;
	width = pallette_zoom - 1; height = pallette_zoom - 1;
	for(auto y = 0; y < 16; y++) {
		caret.y = push.caret.y + y * pallette_zoom;
		for(auto x = 0; x < 16; x++) {
			auto index = y * 16 + x;
			caret.x = push.caret.x + x * pallette_zoom;
			fore = pallette[index];
			rectf();
			if(pallette_current == index)
				rectb_current();
			if(ishilite()) {
				pallette_spot = index;
				status.add("Index: %1i (0x%2.2h), ", pallette_spot, pallette_spot);
				auto v = pallette[pallette_spot];
				status.adds("RGB: %1i %2i %3i", v.r, v.g, v.b);
				rectb_spot();
				if(hkey == MouseLeft && !hpressed)
					execute(cbsetint, pallette_spot, 0, &pallette_current);
			}
		}
	}
}

static void paint_bitdata() {
	pushrect push;
	pushfore push_fore;
	auto push_clip = clipping; setclipall();
	bitdata_spot = point(-10000, -10000);
	width = zoom - 1; height = zoom - 1;
	for(auto y = 0; y < bitdata.height; y++) {
		caret.y = push.caret.y + y * zoom;
		for(auto x = 0; x < bitdata.width; x++) {
			auto index = *bitdata.ptr(x, y);
			fore = pallette[index];
			caret.x = push.caret.x + x * zoom;
			if(caret.x >= clipping.x2)
				break;
			rectf();
			if(ishilite()) {
				bitdata_spot = point(x, y);
				status.adds("Position: %1i %2i", bitdata_spot.x, bitdata_spot.y);
				status.adds("Index: %1i", index);
				rectb_spot();
			}
		}
	}
	clipping = push_clip;
}

static void mouse_input() {
	switch(hkey) {
	case MouseWheelDown: if(zoom > 4) execute(cbsetint, zoom - 1, 0, &zoom); break;
	case MouseWheelUp: if(zoom < 64) execute(cbsetint, zoom + 1, 0, &zoom); break;
	default: break;
	}
}

static void paint_statusbar() {
	pushfore push;
	auto push_caret = caret;
	height = texth() + metrics::padding * 2;
	caret.y = getheight() - height;
	gradv(colors::form, colors::window, 0);
	fore = colors::border;
	line(caret.x + width, caret.y);
	status_rect.x1 = push_caret.x + metrics::padding;
	status_rect.y1 = caret.y + metrics::padding;
	status_rect.x2 = push_caret.x + width - metrics::padding;
	status_rect.y2 = caret.y + height - metrics::padding;
	height = getheight() - height;
	caret = push_caret;
}

static void paint_status_text() {
	if(!status_text[0])
		return;
	pushrect push;
	pushfore push_fore(colors::text);
	caret.x = status_rect.x1;
	caret.y = status_rect.y1;
	width = status_rect.width();
	height = status_rect.height();
	textc(status_text);
}

void mainview() {
	status.clear();
	paint_statusbar();
	fore = colors::form;
	rectf();
	setoffset(metrics::padding, metrics::padding);
	paint_pallette();
	caret.x += pallette_zoom * 16 + metrics::padding;
	width -= pallette_zoom * 16 + metrics::padding;
	paint_bitdata();
	paint_status_text();
	mouse_input();
}

int main() {
	draw::initialize(0);
	setcaption(szfname("Font edit"));
	set_dark_theme();
	metrics::padding = 4;
	draw::scene(mainview, 0);
	return 0;
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main();
}