#pragma once

#include "flagable.h"
#include "nameable.h"
#include "ownerable.h"

enum orgstat_s : unsigned char {
	Reputation, Influence, Sleeping
};
typedef flagable<4> organizationf;

struct creature;
struct orgstati : nameable {
};
struct orgstatable {
	char abilities[Sleeping + 1];
};
struct organizationi : nameable, ownerable<creature>, orgstatable {
	organizationf allies, enemies;
};

void start_game_organization();
void start_game_reputation();
