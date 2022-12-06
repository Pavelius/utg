#pragma once
#include "variant.h"

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
	static bool isallow(variant v);
	static bool isallow(variants source);
	static void setforeach(int bonus, int param);
	static void run(const char* id);
	static void run(variant v);
	static void run(const variants& source);
};
