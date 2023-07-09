#include "nameable.h"
#include "querry.h"
#include "variant.h"

#pragma once

enum actionstate_s : unsigned char {
	NoActionState,
	ShipOnOrbit, ShipOnPlanet,
	YouOnMarket
};
struct actionstatei : nameable {
};
struct actioni : nameable {
	actionstate_s	state, back;
	variants		condition, effect;
	const char*		cancel;
};
extern actioni* last_action;
extern actionstate_s last_action_state;
