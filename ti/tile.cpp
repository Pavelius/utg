#include "bsdata.h"
#include "system.h"

BSDATA(tilei) = {
	{"NoSpecialTile"},
	{"WormholeAlpha"},
	{"WormholeBeta"},
	{"AsteroidField"},
	{"Nebula"},
	{"Supernova"},
	{"GravityRift"},
};
assert_enum(tilei, GravityRift);