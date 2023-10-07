#include "nameable.h"

#pragma once

typedef bool(*fnvisible)(const void* object);

struct filteri : nameable {
	fnvisible		proc;
};
