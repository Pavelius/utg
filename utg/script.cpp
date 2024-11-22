#include "bsreq.h"
#include "script.h"
#include "list.h"

BSMETA(script) = {
	BSREQ(id),
	{}};
variant* script_begin;
variant* script_end;
script*	last_script;

const char* last_id;

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
	auto p = bsdata<listi>::elements + value;
	script_run(p->id, p->elements);
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

void script_list(const char* id) {
	auto p = bsdata<listi>::find(id);
	if(p)
		script_run(p->id, p->elements);
}

void script_run(variant v) {
	bsdata<varianti>::elements[v.type].pscript(v.value, v.counter);
}

void script_run() {
	while(script_begin < script_end)
		script_run(*script_begin++);
}

void script_run(const variants& source) {
	auto push_begin = script_begin;
	auto push_end = script_end;
	script_begin = source.begin();
	script_end = source.end();
	script_run();
	script_end = push_end;
	script_begin = push_begin;
}

void script_run(const char* id, const variants& source) {
	auto push = last_id; last_id = id;
	script_run(source);
	last_id = push;
}

bool script_allow(variant v) {
	auto& ei = bsdata<varianti>::elements[v.type];
	return ei.ptest ? ei.ptest(v.value, v.counter) : true;
}

bool script_allow(const variants& source) {
	auto push_begin = script_begin;
	auto push_end = script_end;
	script_begin = source.begin();
	script_end = source.end();
	auto result = true;
	while(script_begin < script_end) {
		if(!script_allow(*script_begin++)) {
			result = false;
			break;
		}
	}
	script_end = push_end;
	script_begin = push_begin;
	return result;
}

variants script_body() {
	auto s1 = bsdata<variant>::source.indexof(script_begin);
	if(s1 == -1)
		return {};
	return variants(s1, script_end - script_begin);
}