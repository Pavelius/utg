#pragma once

#include "nameable.h"

struct conditioni : nameable {
	typedef bool(*fntest)();
	fntest proc;
};
