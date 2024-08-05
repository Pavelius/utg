#include "draw.h"
#include "draw_object.h"
#include "resable.h"

const short grid_size = 72; // Standart map size

using namespace draw;

point s2i(point v) {
	return {(short)(v.x / grid_size), (short)(v.y / grid_size)};
}

point i2s(point v) {
	return {(short)(v.x * grid_size), (short)(v.y * grid_size)};
}

point mapable::getscreen() const {
	return {(short)(index.x * grid_size), (short)(index.y * grid_size)};
}

point mapable::getscreenc() const {
	return {(short)(index.x * grid_size + grid_size / 2), (short)(index.y * grid_size + grid_size / 2)};
}

point match_grid(point screen) {
	short x = (screen.x / grid_size) * grid_size + grid_size / 2;
	short y = (screen.y / grid_size) * grid_size + grid_size / 2;
	return {x, y};
}