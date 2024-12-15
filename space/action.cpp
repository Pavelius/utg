#include "action.h"
#include "script.h"

actioni*		last_action;
actionstaten	last_action_state;

BSDATA(actionstatei) = {
	{"NoActionState"},
	{"ShipOnOrbit"},
	{"ShipOnPlanet"},
	{"YouInDocks", ShipOnPlanet},
	{"YouOnMarket", ShipOnPlanet},
};
assert_enum(actionstatei, YouOnMarket)

template<> void fnscript<actionstatei>(int index, int bonus) {
	last_action_state = (actionstaten)index;
}