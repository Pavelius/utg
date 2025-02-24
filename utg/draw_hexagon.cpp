#include "draw.h"
#include "draw_hexagon.h"
#include "math.h"

using namespace draw;

extern "C" double round(double v);

namespace {
const double sqrt_3 = 1.732050807568877;
const double cos_30 = 0.86602540378;
struct cube {
	double x, y, z;
};
}

static cube cube_round(cube c) {
	int q = int(round(c.x));
	int r = int(round(c.y));
	int s = int(round(c.z));
	double q_diff = iabs((double)q - c.x);
	double r_diff = iabs((double)r - c.y);
	double s_diff = iabs((double)s - c.z);
	if(q_diff > r_diff and q_diff > s_diff)
		q = -r - s;
	else if(r_diff > s_diff)
		r = -q - s;
	else
		s = -q - r;
	return {(double)q, (double)r, (double)s};
}

static point cube_to_evenr(cube c) {
	auto col = c.x + (c.z + ((int)c.z & 1)) / 2;
	auto row = c.z;
	return {(short)col, (short)row};
}

static cube axial_to_cube(point pt) {
	return {(double)pt.x, (double)(-pt.x - pt.y), (double)pt.y};
}

point draw::h2p(point hex, int size) {
	auto x = (short)(size * sqrt_3 * (hex.x - 0.5 * (hex.y & 1)));
	auto y = (short)(size * 3 / 2 * hex.y);
	return {x, y};
}

point draw::p2h(point pt, int size) {
	double x = pt.x;
	double y = pt.y;
	auto q = ((sqrt_3 / 3.0) * x - (1.0 / 3.0) * y) / (double)size;
	auto r = ((2.0 / 3.0) * (double)pt.y) / (double)size;
	return cube_to_evenr(cube_round({q, -q - r, r}));
}

point draw::fh2p(point hex, int size) {
	auto x = (short)(size * hex.x * 3 / 2);
	auto y = (short)(sqrt_3 * size * (hex.y + 0.5 * (hex.x & 1)));
	return {x, y};
}

point draw::fp2h(point pt, int size) {
	double x = pt.x;
	double y = pt.y;
	auto q = (2.0 * x / 3.0) / (double)size;
	auto r = (-1.0 * x / 3.0 + sqrt_3 * y / 3.0) / (double)size;
	return cube_to_evenr(cube_round({q, -q - r, r}));
}

int draw::getdistance(point h1, point h2) {
	auto a = axial_to_cube(h1);
	auto b = axial_to_cube(h2);
	return int(iabs(a.x - b.x) + iabs(a.y - b.y) + iabs(a.z - b.z)) / 2;
}

static void rectfsized() {
	pushrect push;
	short dy = (short)(fsize * cos_30);
	short dx = fsize / 2;
	caret.x -= dx;
	caret.y -= dy;
	height = dy * 2;
	width = dx * 2;
	rectf();
}

void draw::fhexagonf() {
	rectfsized();
}