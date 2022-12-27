#include "crt.h"
#include "weather.h"

BSDATA(seasoni) = {
	{"Spring"},
	{"Summer"},
	{"Autum"},
	{"Winter"},
};
assert_enum(seasoni, Winter)