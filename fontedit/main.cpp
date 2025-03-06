#include "draw.h"
#include "draw_scroll.h"
#include "editor.h"
#include "io_stream.h"
#include "stringbuilder.h"

using namespace draw;

static char status_text[260];
static rect status_rect;
static stringbuilder status(status_text);

static surface bitdata(32, 32, 8);
static surface bitcopy(32, 32, 8);
static point bitdata_spot;
static int bitdata_oy, bitdata_ox;
static int zoom = 16;

const int pallette_zoom = 14; // Pallette zoom view
static int pallette_spot, pallette_current;

void set_light_theme();
void set_dark_theme();
void open_file();

static void window(fnevent proc) {
	pushfore push_fore(colors::border);
	setoffset(metrics::padding, metrics::padding);
	rectb();
	setoffset(1, 1);
	rectb();
	setoffset(1, 1);
	proc();
}

static void groupv(fnevent proc) {
	auto push_caret = caret;
	auto push_width = width;
	auto push_height = height;
	window(proc);
	caret = push_caret;
	width = push_width;
	caret.y += height + metrics::padding + metrics::padding;
	height = push_height;
}

static void rectb_current() {
	pushrect push_rect;
	pushfore push(colors::active.mix(colors::form));
	rectb();
	setoffset(1, 1);
	rectb();
}

static void setvert(int v) {
	caret.y += v;
	height -= v;
}

static void rectb_spot() {
	pushfore push(colors::active);
	rectb();
}

static void rectb_spot_pallette() {
	pushfore push(pallette[pallette_current]);
	rectb();
}

static void rectf_hilite(unsigned char v) {
	auto push_alpha = alpha; alpha = v;
	pushfore push_fore(colors::button);
	rectf();
	alpha = push_alpha;
}

static void rectf_active() {
	if(hpressed)
		rectf_hilite(64);
	else
		rectf_hilite(128);
	rectb();
}

static void tool(int id, unsigned key, fnevent proc) {
	if(ishilite()) {
		rectf_active();
		if(hkey == MouseLeft && !hpressed)
			execute(proc);
	}
	image(caret.x + width / 2, caret.y + height / 2, metrics::icons, id, 0);
}

static void paint_tool(int size) {
	pushrect push;
	height = size + metrics::padding * 2;
	gradv(colors::form, colors::window);
	caret.y += metrics::padding; height = size;
	caret.x += metrics::padding; width = size;
	tool(0, 0, open_file);
}

static void paint_toolbar() {
	const int size = 24;
	paint_tool(size);
	caret.y += size + metrics::padding * 2; height -= size + metrics::padding;
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

static void raw832(unsigned char* d, int d_scan, unsigned char* s, int s_scan, int width, int height, const color* pallette) {
	const int cbd = 4;
	while(height-- > 0) {
		unsigned char* p1 = d;
		unsigned char* sb = s;
		unsigned char* se = s + width;
		while(sb < se) {
			*((color*)p1) = pallette[*sb++];
			p1 += cbd;
		}
		s += s_scan;
		d += d_scan;
	}
}

static void paint_preview() {
	raw832(canvas->ptr(caret.x, caret.y), canvas->scanline,
		bitdata.ptr(0, 0), bitdata.scanline, bitdata.width, bitdata.height, pallette);
	setvert(metrics::padding + bitdata.height);
}

static void paint_bitdata() {
	pushrect push;
	pushfore push_fore;
	auto push_clip = clipping; setclipall();
	bitdata_spot = point(-10000, -10000);
	width = zoom - 1; height = zoom - 1;
	for(auto y = bitdata_oy; y < bitdata.height; y++) {
		caret.y = push.caret.y + (y - bitdata_oy) * zoom;
		for(auto x = bitdata_ox; x < bitdata.width; x++) {
			auto index = *bitdata.ptr(x, y);
			fore = pallette[index];
			caret.x = push.caret.x + (x - bitdata_ox) * zoom;
			if(caret.x >= clipping.x2)
				break;
			rectf();
			if(ishilite()) {
				bitdata_spot = point(x, y);
				status.adds("Position: %1i %2i", bitdata_spot.x, bitdata_spot.y);
				status.adds("Index: %1i", index);
				rectb_spot_pallette();
				if((hkey == MouseMove || hkey == MouseLeft) && hpressed)
					execute(cbsetuc, pallette_current, 0, bitdata.ptr(x, y));
				if(hkey == MouseRight && !hpressed)
					execute(cbsetint, index, 0, &pallette_current);
			}
		}
	}
	clipping = push_clip;
}

static void paint_bitdata_scroll() {
	input_scroll(bitdata_oy, bitdata.height, zoom);
	input_scroll_horiz(bitdata_ox, bitdata.width, zoom);
	paint_bitdata();
	paint_scroll(bitdata_oy, bitdata.height, zoom);
	paint_scroll_horiz(bitdata_ox, bitdata.width, zoom);
}

static void mouse_input() {
	switch(hkey) {
	case Ctrl | MouseWheelDown: if(zoom > 4) execute(cbsetint, zoom - 1, 0, &zoom); break;
	case Ctrl | MouseWheelUp: if(zoom < 64) execute(cbsetint, zoom + 1, 0, &zoom); break;
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
	height = getheight() - height - push_caret.y;
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
	paint_toolbar();
	setoffset(metrics::padding, metrics::padding);
	paint_preview();
	paint_pallette();
	caret.x += pallette_zoom * 16 + metrics::padding;
	width -= pallette_zoom * 16 + metrics::padding;
	paint_bitdata_scroll();
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