#include "bsdata.h"
#include "draw.h"
#include "draw_marker.h"
#include "draw_strategy.h"

using namespace draw;

BSDATAC(marker, 256)

void marker::paint() const {
	auto push_fore = draw::fore;
	draw::fore = fore;
	if(ishilite(size * 2 / 3, data)) {
		hcursor = cursor::Hand;
		auto push_alpha = alpha; alpha = 64;
		if(hkey == MouseLeft && hpressed)
			alpha = 32;
		field(figure::CircleFill, size);
		alpha = push_alpha;
		if(hkey == MouseLeft && !hpressed)
			execute(buttonparam, (long)hilite_object);
	}
	field(shape, size);
	draw::fore = push_fore;
}