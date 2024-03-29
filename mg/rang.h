#include "nameable.h"
#include "skill.h"

#pragma once

enum rang_s : unsigned char {
	Tenderpaws, Guardmouse, PatrolGuard, PatrolLeader, GuardCapitan,
};
struct rangi : nameable {
	unsigned char	age[2];
	skillds			skills;
	char			party_count;
	char			talented;
	char			convice;
	char			mentors;
	char			specialization;
	char			wises;
	char			trait_tender, trait_leader;
};
