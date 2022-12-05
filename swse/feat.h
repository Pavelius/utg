#pragma once
#include "nameable.h"
#include "flagable.h"
#include "variant.h"

typedef flagable<32> featf;

struct feati : nameable {
	variants	prerequisit, effect;
};