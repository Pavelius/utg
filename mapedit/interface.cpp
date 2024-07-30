#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "map.h"
#include "tile.h"
#include "widget.h"

using namespace draw;

static mapi<16, 16>		terrain, terrain_random;
static unsigned char	terrain_param, terrain_land;
static point			terrain_hilite, terrain_caret;

static void terrain_clear_random() {
	for(short x = 0; x < terrain.mx; x++)
		for(short y = 0; y < terrain.my; y++)
			terrain_random.data[y][x] = (unsigned char)(rand() % 256);
}

void terrain_clear() {
	memset(terrain.data, 0, sizeof(terrain.data));
	terrain_clear_random();
}

point fsh2p(point hex, int size) {
	auto x = (short)(size * hex.x * 3 / 2);
	auto y = (short)(2 * size * (hex.y + 0.5 * (hex.x & 1)));
	return {x, y};
}

void fshexagon() {
	auto pt = caret;
	caret.x += fsize;
	line(pt.x + fsize / 2, pt.y + fsize);
	line(pt.x - fsize / 2, pt.y + fsize);
	line(pt.x - fsize, pt.y);
	line(pt.x - fsize / 2, pt.y - fsize);
	line(pt.x + fsize / 2, pt.y - fsize);
	line(pt.x + fsize, pt.y);
	caret = pt;
}

static void set_current_tile() {
	if(terrain_hilite.y != -1)
		terrain[terrain_hilite] = terrain_land;
}

void tile_landscape(sprite* p, unsigned char param) {
	if(!p || !p->count)
		return;
	image(p, param % p->count, 0);
	auto fr = fsize / 2;
	if(ishilite(fr)) {
		terrain_hilite = terrain_caret;
		circle(fr);
		hcursor = cursor::Hand;
		if(hkey == MouseLeft && hpressed)
			execute(set_current_tile);
	}
}

static void paint_terrain() {
	auto push_caret = caret;
	terrain_hilite = {0, -1};
	for(terrain_caret.x = 0; terrain_caret.x < terrain.mx; terrain_caret.x++) {
		for(terrain_caret.y = 0; terrain_caret.y < terrain.my; terrain_caret.y++) {
			auto n = terrain[terrain_caret];
			caret = fsh2p(terrain_caret, 36) - draw::camera;
			tile_landscape(bsdata<tilei>::elements[n].getres(), terrain_random[terrain_caret]);
		}
	}
	caret = push_caret;
}

static void ui_paint() {
	auto push_clip = clipping;
	setclipall();
	paint_terrain();
	paint_objects();
	clipping = push_clip;
}

void ui_start() {
	draw::scene(ui_paint);
}

static void main_finish() {
	input_camera();
}

void ui_initialize() {
	terrain_clear();
	fsize = 36;
	pfinish = main_finish;
}

static void cbsetuc() {
	auto p = (unsigned char*)hobject;
	*p = (unsigned char)hparam;
}

static void paint_tile(tilei& e, int index) {
	auto p = e.getres();
	if(!p)
		return;
	auto push_clip = clipping;
	auto push_caret = caret;
	setclip({caret.x, caret.y, caret.x + fsize, caret.y + fsize});
	caret.x += fsize / 2; caret.y += fsize / 2;
	image(p, index, 0);
	caret = push_caret;
	clipping = push_clip;
	rectpush push;
	width = fsize; height = fsize;
	if(index == terrain_land)
		rectb();
	if(ishilite()) {
		if(hkey == MouseLeft && hpressed)
			execute(cbsetuc, index, 0, &terrain_land);
	}
}

static void tile_panel_tiles() {
	auto index = 0;
	for(auto& e : bsdata<tilei>()) {
		paint_tile(e, index++);
		caret.x += fsize + 4;
	}
}

static void tile_panel_background() {
	rectpush push;
	auto n = bsdata<tilei>::source.getcount();
	if(n > 10)
		n = 10;
	height = fsize;
	width = ((fsize * 2) + 4) * n;
	swindow(false);
}

static void tile_panel() {
	tile_panel_tiles();
}

void status_info() {
	auto push_caret = caret;
	caret.y += metrics::padding + metrics::border;
	caret.x += metrics::padding + metrics::border;
	tile_panel();
	caret = push_caret;
	caret.y += fsize + (metrics::padding + metrics::border) * 2;
}

BSDATA(widget) = {
	{"TilePanel", tile_panel},
};
BSDATAF(widget)