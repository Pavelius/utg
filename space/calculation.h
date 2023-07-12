#include "nameable.h"

#pragma once

struct stringbuilder;

struct calculationi : nameable {
	typedef int(*fnnum)();
	typedef void(*fnstr)(stringbuilder& result);
	int			width;
	fnstr		pstr;
	fnnum		pnum;
};
extern calculationi* last_calculation;