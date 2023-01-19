#include "action.h"
#include "unit.h"

#pragma once

struct heroi : uniti, moveable {
	const char*	resid;
	actioni*	action;
};
extern heroi* hero;