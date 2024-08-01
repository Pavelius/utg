#include "draw.h"
#include "draw_object.h"
#include "draw_strategy.h"
#include "map.h"
#include "tile.h"
#include "widget.h"

using namespace draw;

const int avatar_size = 32;

static mapi<16, 16>		terrain, terrain_random;
static mapi<16, 16, unsigned> terrain_state;
static unsigned char	terrain_param, terrain_land;
static point			terrain_hilite, terrain_caret;

static int get_terrain_mask(point v, unsigned char t) {
	unsigned char result = 0;
	static point all_0[] = {{-1, -1}, {-1, 0}, {0, 1}, {1, 0}, {1, -1}, {0, -1}};
	static point all_1[] = {{-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {0, -1}};
	auto pz = ((v.x & 1) ? all_1 : all_0);
	for(auto i = 0; i < 6; i++) {
		auto m = v + pz[i];
		if(m.x < 0 || m.x >= terrain.mx || m.y < 0 || m.y >= terrain.my || terrain[m] == t)
			result |= 1;
		if(i<5)
			result <<= 1;
	}
	return result;
}

static bool isflag(point v, int f) {
	return (terrain_state[v] & (1 << f)) != 0;
}

static int get_terrain_mask_state(point v, unsigned char t) {
	unsigned char result = 0;
	static point all_0[] = {{0, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}};
	static point all_1[] = {{0, -1}, {1, -1}, {1, 0}, {0, 1}, {-1, 0}, {-1, -1}};
	for(auto n : ((v.x&1) ? all_1 : all_0)) {
		auto m = n + v;
		if(m.x < 0 || m.x >= terrain.mx || m.y < 0 || m.y >= terrain.my || isflag(m, t))
			result |= 1;
		result <<= 1;
	}
	return result;
}

static void terrain_clear_random() {
	for(short x = 0; x < terrain.mx; x++)
		for(short y = 0; y < terrain.my; y++)
			terrain_random.data[y][x] = (unsigned char)(rand() % 256);
}

void terrain_clear() {
	memset(terrain.data, 0, sizeof(terrain.data));
	terrain_clear_random();
}

static point fsh2p(point hex) {
	auto x = (short)(last_tileset->size.x * hex.x * 3 / 4);
	auto y = (short)(last_tileset->size.y * (hex.y + 0.5 * (hex.x & 1)));
	return {x, y};
}

static void set_current_tile() {
	if(terrain_hilite.y != -1)
		terrain[terrain_hilite] = terrain_land;
}

static void set_current_state() {
	if(terrain_hilite.y != -1)
		terrain_state[terrain_hilite] |= (1<<terrain_land);
}

void tile_landscape(sprite* p, unsigned char param) {
	if(!p || !p->count)
		return;
	image(p, param % p->count, 0);
	auto fr = last_tileset->size.x / 4;
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
			caret = fsh2p(terrain_caret) - draw::camera;
			auto param = get_terrain_mask(terrain_caret, n);
			auto pt = last_tileset->tiles.begin() + n;
			if(pt->border)
				param = pt->border->indecies[param];
			tile_landscape(pt->getres(), param);
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
	auto size = 32;
	auto push_clip = clipping;
	auto push_caret = caret;
	setclip({caret.x, caret.y, caret.x + size, caret.y + size});
	caret.x += size / 2; caret.y += size / 2;
	image(p, last_tileset->avatar_tile, 0);
	caret = push_caret;
	clipping = push_clip;
	rectpush push;
	width = size; height = size;
	if(index == terrain_land)
		rectb();
	if(ishilite()) {
		if(hkey == MouseLeft && hpressed)
			execute(cbsetuc, index, 0, &terrain_land);
	}
}

static void tile_panel_tiles() {
	auto index = 0;
	for(auto& e : last_tileset->tiles) {
		paint_tile(e, index++);
		caret.x += avatar_size + 4;
	}
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
	caret.y += avatar_size + (metrics::padding + metrics::border) * 2;
}

BSDATA(widget) = {
	{"TilePanel", tile_panel},
};
BSDATAF(widget)