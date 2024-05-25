#include "point.h"

#pragma once

enum area_s : unsigned char {
	NoArea, Slash, Circle, Ray, Splash, Spray,
};
struct areai {
	const char*	id;
	point		points[16];
};
