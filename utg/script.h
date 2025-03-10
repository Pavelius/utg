#include "variant.h"

#pragma once

template<typename T> void fnscript(int index, int bonus);
template<typename T> bool fntest(int index, int bonus);

struct script {
	typedef void(*fnrun)(int bonus);
	typedef bool(*fntest)(int bonus);
	const char*	id;
	fnrun		proc;
	fntest		test;
};
extern variant* script_begin;
extern variant* script_end;
extern script* last_script;
extern const char* last_id;

bool choosing_script(int bonus);
bool script_allow(variant v);
bool script_allow(const variants& source);
bool script_isrun();

void conditional_script(int bonus);
void script_list(const char* id);
void script_none(int bonus);
void script_run();
void script_run(variant v);
void script_run(const variants& source);
void script_run(const char* id, const variants& source);
void script_stop();

variants script_body();

const char* script_header();