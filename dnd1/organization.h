#pragma once

#include "flagable.h"
#include "nameable.h"
#include "ownerable.h"

enum orgstat_s : unsigned char {
	Reputation, Influence, Sleeping
};
enum organization_s : unsigned char {
	MiddleKindom, Rebels, Horde, DwarvenKindom, ElvenKindom,
	LawfulGod, ChaoticGod, NatureGod,
	TheivesGuild, MercenaryGuild, MerchantsGuild, WizardCouncil
};

typedef flagable<4> organizationf;

struct creature;
struct orgstati : nameable {
};
struct orgstatable {
	char			abilities[Influence + 1];
};
struct organizationi : nameable, ownerable<creature>, orgstatable {
	organizationf	allies, enemies;
};

void start_game_organization();
void start_game_reputation();
