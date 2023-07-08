#include "nameable.h"
#include "querry.h"
#include "variant.h"

#pragma once

enum actionstate_s : unsigned char {
	NoActionState,
	ShipOnOrbit, ShipOnPlanet,
};
struct actionstatei : nameable {
};
struct actioni : nameable {
	actionstate_s	state, next;
	variants		condition, effect;
	querryi*		querry;
	const char*		cancel;
};
