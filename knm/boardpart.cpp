#include "boardpart.h"
#include "crt.h"

BSDATA(boardparti) = {
	{"PlayerAction"},
	{"PublicArea"},
};
assert_enum(boardparti, PublicArea)