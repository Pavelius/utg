#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "map.h"
#include "tile.h"

using namespace draw;

static mapi<16, 16>		terrain, random_seed;
static unsigned char	terrain_tile;
static point			terrain_hilite;

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
		terrain[terrain_hilite] = terrain_tile;
}

void tile_landscape(int x, int y) {
	auto p = gres("Mountains", "art/images");
	if(!p || !p->count)
		return;
	image(p, (y + x) % p->count, 0);
	auto fr = fsize / 2;
	if(ishilite(fr)) {
		terrain_hilite.x = x;
		terrain_hilite.y = y;
		circle(fr);
		hcursor = cursor::Hand;
		if(hkey == MouseLeft && hpressed)
			execute(set_current_tile);
	}
}

static void paint_terrain() {
	auto push_caret = caret;
	terrain_hilite = {0, -1};
	for(short x = 0; x < terrain.mx; x++) {
		for(short y = 0; y < terrain.my; y++) {
			auto n = terrain.data[y][x];
			caret = fsh2p({x, y}, 36) - draw::camera;
			tile_landscape(x, y);
		}
	}
	caret = push_caret;
}

static void ui_paint() {
	paint_terrain();
	paint_objects();
}

void ui_start() {
	draw::scene(ui_paint);
}

static void main_finish() {
	input_camera();
}

void ui_initialize() {
	fsize = 36;
	pfinish = main_finish;
}

void status_info() {
}