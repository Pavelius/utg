#include "bsdata.h"
#include "planet_trait.h"

BSDATA(planet_traiti) = {
	{"NoTrait"},
	{"Cultural"},
	{"Hazardous"},
	{"Industrial"},
};
assert_enum(planet_traiti, Industrial)