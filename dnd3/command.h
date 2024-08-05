#pragma once
#include "nameable.h"

typedef void (*fnevent)();

struct commandi : nameable {
	fnevent		proc;
};
