#pragma once
#include "nameable.h"
#include "flagable.h"
#include "variant.h"

typedef flagable<32> featable;

struct feati : nameable {
	variants	prerequisit, effect;
};
