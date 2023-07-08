#include "action.h"

BSDATA(actionstatei) = {
	{"NoActionState"},
	{"ShipOnOrbit"},
	{"ShipOnPlanet"},
	{"YouOnMarket"},
};
assert_enum(actionstatei, YouOnMarket)

actioni* last_action;
actionstate_s last_action_state;