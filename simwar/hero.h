#include "action.h"
#include "unit.h"

#pragma once

struct heroi : uniti, moveable {
	const char*	resid;
	actioni*	action;
	int			get(cost_s v) const { return effect[v]; }
};
extern heroi* hero;