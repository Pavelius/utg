#include "stringbuilder.h"
#include "move.h"
#include "nameable.h"

static void script_info(const variants& elements, stringbuilder& sb) {
	for(auto v : elements) {
		auto& ei = bsdata<varianti>::elements[v.type];
		auto p = (nameable*)ei.source->ptr(v.value);
		sb.adds(p->getname());
		if(v.counter)
			sb.add("%+1i", v.counter);
	}
}

void moveoptioni::getinfo(stringbuilder& sb) const {
	script_info(effect, sb);
}