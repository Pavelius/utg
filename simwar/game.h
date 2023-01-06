#include "building.h"
#include "player.h"
#include "province.h"
#include "stringbuilder.h"

#pragma once

int get_income(const playeri* p, cost_s v, stringbuilder* psb);

void build_bulding(const playeri* p, const buildingi* pb);
