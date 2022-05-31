#include "function.h"
#include "script.h"

script::fnapply		script::custom;
script::fnapply		script::foreach;
bool				script::stop;

void script::run(variant v) {
	if(v.iskind<script>()) {
		auto p = bsdata<script>::elements + v.value;
		p->proc(v.counter, p->param);
	} if(v.iskind<function>())
		run(bsdata<function>::elements[v.value].script);
	else if(custom)
		custom(v);
}

void script::setforeach(int bonus, int param) {
	foreach = (fnapply)param;
}

void apply_foreach(variant v) {
	if(script::foreach) {
		script::foreach(v);
		script::foreach = 0;
	} else
		script::run(v);
}

void script::run(variants source) {
	auto push_foreach = foreach;
	foreach = 0;
	for(auto v : source) {
		if(stop)
			break;
		apply_foreach(v);
	}
	foreach = push_foreach;
}