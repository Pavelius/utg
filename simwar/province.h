#include "combatable.h"
#include "crt.h"
#include "landscape.h"
#include "point.h"
#include "player.h"

#pragma once

struct provincei : nameable, combatable {
	point				position;
	playeri*			owner;
	const landscapei*	landscape;
	costa				income;
};
struct moveable {
	provincei*			province;
	playeri*			owner;
};