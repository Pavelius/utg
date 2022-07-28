#include "draw.h"
#include "draw_object.h"
#include "main.h"

using namespace draw;

static void aftercreate_object(object* po) {
	if(bsdata<planeti>::have(po->data)) {
		auto p = (planeti*)po->data;
		po->shape = figure::CircleFill;
		po->size = xrand(5, 10);
		po->fore = colors::green;
	} else if(bsdata<systemi>::have(po->data)) {
		po->shape = figure::CircleFill;
		po->size = 32;
		po->fore = colors::yellow;
	}
}

void initialize_interface() {
	object::aftercreate = aftercreate_object;
}