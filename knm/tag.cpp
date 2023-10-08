#include "crt.h"
#include "tag.h"
#include "taga.h"

tag_s last_tag;

BSDATA(tagi) = {
	{"Used"},
	{"Fast"},
	{"Fly"},
	{"Forest"},
	{"Mountains"},
	{"Farmlands"},
};
assert_enum(tagi, Farmlands)