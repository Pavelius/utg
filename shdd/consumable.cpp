#include "bsreq.h"
#include "crt.h"
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