#include "avatarable.h"
#include "point.h"

#pragma once

struct mapable {
	point			index;
	point			getscreen() const;
	point			getscreenc() const;
	const bool operator==(const mapable& v) const { return index == v.index; }
	const bool operator!=(const mapable& v) const { return index != v.index; }
};
struct resable : mapable, avatarable {
	short unsigned	flags;
};

point i2s(point v);
point match_grid(point screen);
point s2i(point v);
