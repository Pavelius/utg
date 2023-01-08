#include "statable.h"
#include "crt.h"
#include "landscape.h"
#include "point.h"

#pragma once

struct playeri;

struct provincei : nameable {
	playeri*			owner;
	point				position;
	const landscapei*	landscape;
	costa				income;
	void				paint() const;
};
struct moveable {
	playeri*			player;
	provincei*			province;
};
extern provincei*		province;