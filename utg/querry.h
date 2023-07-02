#include "nameable.h"

#pragma once

struct querryi : nameable {
	typedef void(*fnselect)(int bonus);
	fnselect	proc;
};
