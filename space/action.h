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
	actionstate_s	state;
	variants		condition, effect;
};
extern actioni* last_action;
