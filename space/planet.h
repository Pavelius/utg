#include "landscape.h"
#include "system.h"

#pragma once

struct planeti : nameable, drawable, planetr {
	systemi*	system;
	landscapei*	landscape;
	void		paint() const;
};