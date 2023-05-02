#include "statable.h"

#pragma once

struct creature : statable {
	statable	basic;
	char		ancestry;
	void		update();
};
extern creature* player;