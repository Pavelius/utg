#include "crt.h"
#include "tag.h"
#include "taga.h"

tag_s last_tag;

BSDATA(tagi) = {
	{"Used"},
	{"Infantry"},
	{"Fast"},
	{"Fly"},
	{"Forest"},
	{"Mountains"},
	{"Farmlands"},
	{"Sea"},
};
assert_enum(tagi, Sea)