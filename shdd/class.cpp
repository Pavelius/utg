#include "bsdata.h"
#include "class.h"

classi* last_class;

BSDATA(classi) = {
	{"NoClass", 6},
	{"Fighter", 8},
	{"Priest", 6},
	{"Theif", 4},
	{"Wizard", 4},
};
assert_enum(classi, Wizard)