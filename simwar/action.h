#include "nameable.h"

#pragma once

struct actioni : nameable {
	typedef void(*fnaction)();
	typedef bool(*fntest)(const void* object);
	fnaction	proc;
	fntest		test;
	const char*	next;
	char		priority; // 0 - 10
	//bool		is(actionfs v) const { return (flags & (1 << v)) != 0; }
};
extern actioni* action;