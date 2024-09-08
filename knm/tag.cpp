#include "bsdata.h"
#include "tag.h"
#include "taga.h"

tag_s last_tag;

BSDATA(tagi) = {
	{"Used"},
	{"Fly"},
	{"AnimateSkeletons"},
	{"Forest"},
	{"Mountains"},
	{"Farmlands"},
	{"Sea"},
};
assert_enum(tagi, Sea)