#include "landscape.h"
#include "system.h"

#pragma once

struct planeti : posable, planetr {
	systemi*	system;
	landscapei*	landscape;
	void		paint() const;
};