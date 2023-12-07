#include "nameable.h"
#include "flagable.h"
#include "variant.h"

#pragma once

typedef flagable<32> featf;

struct feati : nameable {
	variant		upgrade;
	variants	prerequisit, effect;
};