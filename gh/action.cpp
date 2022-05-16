#include "main.h"

BSDATA(actioni) = {
	{"Apply"},
	{"Shield"},
	{"Retaliate"},
	{"Move"},
	{"Attack"},
	{"Push"},
	{"Pull"},
	{"Heal"},
	{"DisarmTrap"},
	{"Loot"},
	{"Bless"},
	{"Curse"},
	{"RecoverDiscarded"},
	{"TrapDamage"}, {"TrapPoisonDamage"},
	{"Discard"},	
};
assert_enum(actioni, Discard)

void action::clear() {
	memset(this, 0, sizeof(*this));
}

static action* addnew(action* p, action* pb) {
	return p ? p + 1 : pb;
}

const variant* action::parse_modifier(const variant* p, const variant* pe) {
	while(p < pe) {
		if(p->iskind<modifieri>()) {
			modifiers[p->value] += p->counter;
			p++;
		} else if(p->iskind<elementi>())
			gather.set(p->value);
		else if(p->iskind<statei>())
			you.set(p->value);
		else if(p->iskind<areai>())
			area = (area_s)p->value;
	}
	return p;
}

int gamei::parse(variants source, action* pb) {
	action* p = 0;
	for(auto v : source) {
		if(v.iskind<statei>()) {
			p = addnew(p, pb);
			p->type = Apply;
			p->you.set(v.value);
		}
	}
}