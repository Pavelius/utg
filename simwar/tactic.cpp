#include "crt.h"
#include "tactic.h"

bool tactici::isdisable(const tactici* v) const {
	return disable.is(getbsi(v));
}

void tactica::add(const tactici* v) {
	if(find(const_cast<tactici*>(v)))
		return;
	collection<tactici>::add(const_cast<tactici*>(v));
}

void tactica::add(const tacticaf& v) {
	int count = bsdata<tactici>::source.getcount();
	for(int i = 0; i < count; i++) {
		if(!v.is(i))
			continue;
		add(bsdata<tactici>::elements + i);
	}
}