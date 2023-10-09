#include "crt.h"
#include "tag.h"
#include "taga.h"

tag_s last_tag;

BSDATA(tagi) = {
	{"Used"},
	{"Infantry"},
	{"Fly"},
	{"AnimateSkeletons"},
	{"Forest"},
	{"Mountains"},
	{"Farmlands"},
	{"Sea"},
};
assert_enum(tagi, Sea)