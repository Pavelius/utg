#include "script.h"
#include "list.h"

variant* script_begin;
variant* script_end;

template<> void fnscript<script>(int value, int bonus) {
	bsdata<script>::elements[value].proc(bonus);
}
template<> bool fntest<script>(int value, int bonus) {
	auto p = bsdata<script>::elements + value;
	if(p->test)
		return p->test(bonus);
	return true;
}

template<> void fnscript<listi>(int value, int bonus) {
	script::run(bsdata<listi>::elements[value].elements);
}
template<> bool fntest<listi>(int value, int bonus) {
	return script::allow(bsdata<listi>::elements[value].elements);
}

void script::stop() {
	script_begin = script_end;
}

void script::run(const char* id, int bonus) {
	auto p = bsdata<script>::find(id);
	if(p)
		p->proc(bonus);
}

void script::run(variant v) {
	auto& ei = bsdata<varianti>::elements[v.type];
	if(ei.pscript)
		ei.pscript(v.value, v.counter);
}

void script::run(const variants& source) {
	auto push_begin = script_begin;
	auto push_end = script_end;
	script_begin = source.begin();
	script_end = source.end();
	while(script_begin < script_end)
		run(*script_begin++);
	script_end = push_end;
	script_begin = push_begin;
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