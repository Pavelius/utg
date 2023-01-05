#include "combatable.h"
#include "crt.h"
#include "landscape.h"
#include "player.h"

#pragma once

struct provincei : nameable, combatable {
	const landscapei*	landscape;
};
struct moveable {
	provincei*			province;
	playeri*			owner;
};