#include "variant.h"

#pragma once

struct script {
	typedef void(*fnscript)(int bonus, int param);
	typedef void(*fnapply)(variant v);
	typedef bool(*fntest)(variant v, bool& need_stop);
	const char*	id;
	fnscript	proc;
	int			param;
	static fnapply prun;
	static fntest ptest;
	static variants elements;
	static bool stop;
	static bool isallow(variants source);
	static void setforeach(int bonus, int param);
	static void run(const char* id);
	static void run(variant v);
	static void run(const variants& source);
};
extern void runscript(variant v);
extern void runscript(const variants& source);
