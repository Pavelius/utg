#include "nameable.h"
#include "statable.h"

#pragma once

struct actioni : nameable {
	typedef void(*fnaction)();
	typedef bool(*fntest)(const void* object);
	fnaction	proc;
	fntest		test;
	char		priority; // 0 - 10
	char		mobilize;
	costa		cost;

};
extern actioni* action;