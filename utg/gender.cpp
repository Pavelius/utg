#include "bsreq.h"
#include "gender.h"

gendern last_gender;

BSMETA(genderi) = {
	BSREQ(id),
	{}};
BSDATA(genderi) = {
	{"NoGender"},
	{"Male"},
	{"Female"},
};
assert_enum(genderi, Female)