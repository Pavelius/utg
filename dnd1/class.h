#include "ability.h"
#include "race.h"
#include "organization.h"

#pragma once

struct classi : nameable {
	abilityn		prime;
	racei*			origin;
	char			minimal[6];
	char			hd, magic;
	unsigned		experience[20];
	unsigned		allow;
	organizationf	reputation;
};