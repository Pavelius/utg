#include "bsreq.h"
#include "bsdata.h"
#include "consumable.h"

BSMETA(consumablei) = {
	BSREQ(id),
	{}};
BSDATA(consumablei) = {
	{"CP"},
	{"SP"},
	{"GP"},
	{"Experience"},
};
assert_enum(consumablei, Experience)