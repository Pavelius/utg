#include "draw.h"
#include "draw_object.h"
#include "resable.h"

const short grid_size = 72;

using namespace draw;

point mapable::getscreen() const {
	return {(short)(index.x * grid_size), (short)(index.y * grid_size)};
}