#include "combatable.h"
#include "crt.h"
#include "landscape.h"
#include "point.h"

#pragma once

struct playeri;

struct provincei : nameable, combatable {
	playeri*			owner;
	point				position;
	const landscapei*	landscape;
	costa				income;
};
struct moveable {
	playeri*			player;
	provincei*			province;
};
extern provincei*		province;