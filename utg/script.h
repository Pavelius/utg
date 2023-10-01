#include "variant.h"

#pragma once

template<typename T>
void fnscript(int index, int bonus);

template<typename T>
bool fntest(int index, int bonus);

struct script {
	typedef void(*fnrun)(int bonus);
	typedef bool(*fntest)(int bonus);
	const char*	id;
	fnrun		proc;
	fntest		test;
	static bool allow(variant v);
	static bool allow(const variants& source);
};
void conditional_script(int bonus);
extern variant* script_begin;
extern variant* script_end;
extern script* last_script;

bool script_isrun();
void script_run(variant v);
void script_run(const variants& source);
void script_run(const char* id, int bonus);
void script_stop();