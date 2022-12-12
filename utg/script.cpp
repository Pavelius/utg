#include "script.h"
#include "list.h"

script::fnapply script::apply;
bool script::stop;

template<> void fnscript<script>(int value, int bonus) {
	bsdata<script>::elements[value].proc(bonus);
}

template<> void fnscript<listi>(int value, int bonus) {
	script::run(bsdata<listi>::elements[value].elements);
}

void script::run(variant v) {
	auto& ei = bsdata<varianti>::elements[v.type];
	if(ei.pscript)
		ei.pscript(v.value, v.counter);
}

void script::run(const variants& source) {
	if(stop)
		return;
	auto push_stop = stop;
	auto push_apply = apply;
	for(auto v : source) {
		if(stop)
			break;
		if(apply) {
			apply(v);
			apply = 0;
		} else
			run(v);
	}
	apply = push_apply;
	stop = push_stop;
}

bool script::allow(variant v) {
	auto& ei = bsdata<varianti>::elements[v.type];
	return ei.ptest ? ei.ptest(v.value, v.counter) : true;
}

bool script::allow(const variants& source) {
	for(auto v : source) {
		if(!allow(v))
			return false;
	}
	return true;
}