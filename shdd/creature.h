#include "class.h"
#include "race.h"
#include "statable.h"
#include "wearable.h"

#pragma once

struct creature : statable, wearable {
	class_s		kind;
	race_s		ancestry;
	statable	basic;
	bool		isallow(const item& v) const;
	bool		isplayer() const { return true; }
	void		update();
};
extern creature* player;