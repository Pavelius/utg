#include "resable.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct creature : resable, statable, wearable {
	statable	basic;
	void		clear();
	void		create();
	void		paint() const;
	void		update();
};
extern creature* player;
