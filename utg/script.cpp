#include "bsreq.h"
#include "script.h"
#include "list.h"

BSMETA(script) = {
	BSREQ(id),
	{}};
variant* script_begin;
variant* script_end;
script*	last_script;

template<> bool fntest<script>(int value, int bonus) {
	last_script = bsdata<script>::elements + value;
	if(last_script->test)
		return last_script->test(bonus);
	return true;
}
template<> void fnscript<script>(int value, int bonus) {
	last_script = bsdata<script>::elements + value;
	last_script->proc(bonus);
}

template<> bool fntest<listi>(int value, int bonus) {
	return script_allow(bsdata<listi>::elements[value].elements);
}
template<> void fnscript<listi>(int value, int bonus) {
	auto push_list = last_list;
	last_list = bsdata<listi>::elements + value;
	script_run(last_list->elements);
	last_list = push_list;
}

void script_none(int bonus) {
}

void conditional_script(int bonus) {
	if(!last_script->test)
		return;
	if(!last_script->test(bonus))
		script_stop();
}

bool choosing_script(int bonus) {
	last_script->proc(bonus);
	return true;
}

void script_stop() {
	script_begin = script_end;
}

bool script_isrun() {
	return script_end > script_begin;
}

void script_run(const char* id, int bonus) {
	last_script = bsdata<script>::find(id);
	if(last_script)
		last_script->proc(bonus);
}

void script_run(variant v) {
	bsdata<varianti>::elements[v.type].pscript(v.value, v.counter);
}

void script_run(const variants& source) {
	auto push_begin = script_begin;
	auto push_end = script_end;
	script_begin = source.begin();
	script_end = source.end();
	while(script_begin < script_end)
		script_run(*script_begin++);
	script_end = push_end;
	script_begin = push_begin;
}

bool script_allow(variant v) {
	auto& ei = bsdata<varianti>::elements[v.type];
	return ei.ptest ? ei.ptest(v.value, v.counter) : true;
}

bool script_allow(const variants& source) {
	for(auto v : source) {
		if(!script_allow(v))
			return false;
	}
	return true;
}