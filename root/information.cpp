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

template<> void ftstatus<moveoptioni>(const void* object, stringbuilder& sb) {
	auto p = (moveoptioni*)object;
	script_info(p->effect, sb);
}