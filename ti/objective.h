#include "nameable.h"
#include "variant.h"

#pragma once

struct objectivei : nameable {
	char		stage, value;
	variants	condition;
	variants	hits;
};