#include "action.h"

BSDATA(actionstatei) = {
	{"NoActionState"},
	{"ShipOnOrbit"},
	{"ShipOnPlanet"},
};
assert_enum(actionstatei, ShipOnPlanet)

actioni* last_action;
actionstate_s last_action_state;