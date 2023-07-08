#include "action.h"

BSDATA(actionstatei) = {
	{"NoActionState"},
	{"ShipOnOrbit"},
	{"ShipOnPlanet"},
};
assert_enum(actionstatei, ShipOnPlanet)