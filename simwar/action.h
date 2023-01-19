#include "nameable.h"

#pragma once

struct actioni : nameable {
	typedef void(*fnaction)();
	typedef bool(*fntest)(const void* object);
	fnaction	proc;
	fntest		test;
	char		priority; // 0 - 10
};
extern actioni* action;