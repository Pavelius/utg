#include "landscape.h"
#include "system.h"

#pragma once

struct planeti : nameable, drawable, planetr {
	short unsigned	system;
	short unsigned	landscape;
	void			paint() const;
	landscapei*		getlandscape() const;
	systemi*		getsystem() const;
	void			update();
};
extern planeti* last_planet;