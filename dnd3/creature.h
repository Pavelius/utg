#include "resable.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct creature : resable, statable, wearable {
	void		clear();
	void		create();
	void		paint() const;
};
extern creature* player;
