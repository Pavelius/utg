#include "condition.h"
#include "list.h"
#include "script.h"

script::fnapply script::prun;
script::fntest script::ptest;
static script::fnapply foreach;
bool script::stop;

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

void script::setforeach(int bonus, int param) {
	foreach = (fnapply)param;
}

void script::run(const variants& source) {
	auto push_foreach = foreach; foreach = 0;
	script::stop = false;
	for(auto v : source) {
		if(script::stop)
			break;
		if(foreach) {
			foreach(v);
			foreach = 0;
		} else
			run(v);
	}
	foreach = push_foreach;
}

void script::run(variant v) {
	if(v.iskind<script>()) {
		auto p = bsdata<script>::elements + v.value;
		p->proc(v.counter, p->param);
	} else if(v.iskind<listi>())
		script::run(bsdata<listi>::elements[v.value].elements);
	else if(v.iskind<conditioni>()) {
		auto p = bsdata<conditioni>::elements + v.value;
		if(v.counter >= 0) {
			if(!p->proc(v.counter, p->param))
				script::stop = true;
		} else {
			if(p->proc(-v.counter, p->param))
				script::stop = true;
		}
	} else if(script::prun)
		script::prun(v);
}

void script::run(const char* id) {
	auto p1 = bsdata<listi>::find(id);
	if(p1) {
		script::run(p1->elements);
		return;
	}
	auto p2 = bsdata<script>::find(id);
	if(p2) {
		p2->proc(0, p2->param);
		return;
	}
}