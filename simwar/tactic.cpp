#include "crt.h"
#include "tactic.h"

bool tactici::isdisable(const tactici* v) const {
	return disable.is(getbsi(v));
}

void tactica::add(const tactici* v) {
	auto cv = const_cast<tactici*>(v);
	if(find(cv) != -1)
		return;
	if(cv->upgrade) {
		auto uv = const_cast<tactici*>(cv->upgrade);
		auto i = find(uv);
		if(i != -1) {
			data[i] = uv;
			return;
		}
	}
	collection<tactici>::add(cv);
}

void tactica::add(const tacticaf& v) {
	int count = bsdata<tactici>::source.getcount();
	for(int i = 0; i < count; i++) {
		if(!v.is(i))
			continue;
		add(bsdata<tactici>::elements + i);
	}
}