#include "nameable.h"
#include "querry.h"
#include "variant.h"

#pragma once

enum actionstaten : unsigned char {
	NoActionState,
	ShipOnOrbit, ShipOnPlanet,
	YouInDocks, YouOnMarket
};
struct actionstatei : nameable {
	actionstaten cancel_state;
	const char*	cancel;
};
struct actioni : nameable {
	actionstaten state, back;
	variants	condition, effect;
	const char*	cancel;
};
extern actioni* last_action;
extern actionstaten last_action_state;
