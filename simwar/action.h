#include "nameable.h"
#include "statable.h"

#pragma once

struct actioni : nameable {
	typedef void(*fnaction)();
	typedef bool(*fntest)(const void* object);
	fnaction	proc;
	fntest		test;
	costa		cost;

};
extern actioni* action;