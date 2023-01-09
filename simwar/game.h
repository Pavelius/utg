#include "building.h"
#include "calendar.h"
#include "player.h"
#include "province.h"
#include "stringbuilder.h"

#pragma once

struct gamei : calendari {
	static int		getincome(cost_s v, stringbuilder* psb);
	static void		build(const buildingi* pb);
};
extern gamei game;

