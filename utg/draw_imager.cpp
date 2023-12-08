#include "draw.h"

using namespace draw;

static void raw32cr(int x, int y, int width, int x0, int y0, unsigned char* s, unsigned scanline) {
	if(y >= clipping.y2 || y < clipping.y1 || x >= clipping.x2)
		return;
	auto x2 = x + width;
	if(x2 < clipping.x1)
		return;
	if(x < clipping.x1)
		x = clipping.x1;
	if(x2 > clipping.x2)
		x2 = clipping.x2;
	width = x2 - x;
	if(width <= 0)
		return;
	const auto cbs = 3;
	const auto cbd = 4;
	auto p1 = ptr(x, y);
	auto sb = s + ((y - y0) * scanline + (x - x0)) * cbs;
	auto se = sb + width * cbs;
	while(sb < se) {
		p1[0] = sb[0];
		p1[1] = sb[1];
		p1[2] = sb[2];
		sb += cbs;
		p1 += cbd;
	}
}

void circle_image(int xm, int ym, const sprite* e, int id, int r) {
	if(!e)
		return;
	auto& f = e->get(id);
	if(!f.offset)
		return;
	if(r > f.sx / 2)
		r = f.sx / 2;
	if(r > f.sy / 2)
		r = f.sy / 2;
	if(xm - r >= clipping.x2 || xm + r < clipping.x1 || ym - r >= clipping.y2 || ym + r < clipping.y1)
		return;
	auto sx = f.sx;
	auto x1 = xm - f.sx / 2;
	auto y1 = ym - f.sy / 2;
	unsigned char* s = (unsigned char*)e + f.offset;
	int x = -r, y = 0, err = 2 - 2 * r, y2 = -1000;
	do {
		if(y2 != y) {
			y2 = y;
			raw32cr(xm + x, ym + y, -2 * x, x1, y1, s, sx);
			if(y != 0)
				raw32cr(xm + x, ym - y, -2 * x, x1, y1, s, sx);
		}
		r = err;
		if(r <= y)
			err += ++y * 2 + 1;
		if(r > x || err > y)
			err += ++x * 2 + 1;
	} while(x < 0);
}