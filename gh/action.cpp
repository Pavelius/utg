#include "main.h"

BSDATA(actioni) = {
	{"Shield"},
	{"Retaliate"},
	{"Move"},
	{"Attack"},
	{"Push"},
	{"Pull"},
	{"Heal"},
	{"DisarmTrap"},
	{"Loot"},
	{"Kill"},
	{"Bless"},
	{"Curse"},
	{"RecoverDiscarded"},
	{"Discard"},	
};
assert_enum(actioni, Discard)

void action::clear() {
	memset(this, 0, sizeof(*this));
}

static action* addnew(action* p, action* pb) {
	return p ? p + 1 : pb;
}