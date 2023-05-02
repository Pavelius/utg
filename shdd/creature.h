#include "class.h"
#include "item.h"
#include "race.h"
#include "statable.h"

#pragma once

struct creature : statable {
	class_s		kind;
	race_s		ancestry;
	statable	basic;
	bool		isallow(const item& v) const;
	bool		isplayer() const { return true; }
	void		update();
};
extern creature* player;