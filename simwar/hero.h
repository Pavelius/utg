#include "actable.h"
#include "action.h"
#include "site.h"

#pragma once

struct heroi : actable, moveable {
	const char*	resid;
	actioni*	action;
	site*		location;
	char		wounds;
	int			get(cost_s v) const { return 0; }
};
extern heroi* hero;