#include "crt.h"
#include "draw.h"
#include "draw_marker.h"
#include "draw_strategy.h"

using namespace draw;

BSDATAC(marker, 256)

void marker::paint() const {
	auto push_fore = draw::fore;
	draw::fore = fore;
	if(ishilite(size * 2 / 3, data)) {
		hot.cursor = cursor::Hand;
		auto push_alpha = alpha; alpha = 64;
		if(hot.key == MouseLeft && hot.pressed)
			alpha = 32;
		field(figure::CircleFill, size);
		alpha = push_alpha;
		if(hot.key == MouseLeft && !hot.pressed)
			execute(buttonparam, (long)hilite_object);
	}
	field(shape, size);
	draw::fore = push_fore;
}