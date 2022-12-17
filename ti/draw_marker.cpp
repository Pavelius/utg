#include "crt.h"
#include "draw.h"
#include "draw_marker.h"

using namespace draw;

BSDATAC(marker, 256)

void marker::paint() const {
	auto push_fore = fore;
	draw::fore = fore;
	field(shape, size);
	draw::fore = push_fore;
}