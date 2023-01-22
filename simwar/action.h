#include "nameable.h"

#pragma once

struct actioni : nameable {
	typedef void(*fnaction)();
	typedef bool(*fntest)(const void* object);
	fnaction	proc;
	fntest		test;
	const char*	next;
	char		priority; // 0 - 10
	char		mobilize;
};
extern actioni* action;