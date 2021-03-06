#include "condition.h"
#include "function.h"
#include "script.h"

script::fnapply		script::prun;
script::fntest		script::ptest;
script::fnapply		script::foreach;
bool				script::stop;

bool script::isallow(variants source) {
	for(auto v : source) {
		if(v.iskind<conditioni>()) {
			auto p = bsdata<conditioni>::elements + v.value;
			if(v.counter >= 0) {
				if(!p->proc(v.counter, p->param))
					return false;
			} else {
				if(p->proc(-v.counter, p->param))
					return false;
			}
		} else if(ptest) {
			auto allowed = true;
			if(!ptest(v, allowed))
				break;
			return allowed;
		} else
			break;
	}
	return true;
}

void script::run(variant v) {
	if(v.iskind<script>()) {
		auto p = bsdata<script>::elements + v.value;
		p->proc(v.counter, p->param);
	} else if(v.iskind<function>()) {
		auto push_last = function::last;
		run(bsdata<function>::elements[v.value].script);
		function::last = push_last;
	} else if(v.iskind<conditioni>()) {
		auto p = bsdata<conditioni>::elements + v.value;
		if(v.counter >= 0) {
			if(!p->proc(v.counter, p->param))
				stop = true;
		} else {
			if(p->proc(-v.counter, p->param))
				stop = true;
		}
	} else if(prun)
		prun(v);
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
	stop = false;
	for(auto v : source) {
		if(stop)
			break;
		apply_foreach(v);
	}
	foreach = push_foreach;
}

void script::run(const char* id) {
	auto p = bsdata<function>::find(id);
	if(p) {
		run(p->script);
		return;
	}
}