#include "skill.h"

#pragma once

enum rang_s : unsigned char {
	Tenderpaws, Guardmouse, PatrolGuard, PatrolLeader, GuardCapitan,
};
struct rangi {
	const char*		id;
	unsigned char	age[2];
	skillds			skills[16];
	char			talented;
	char			convice;
	char			mentors;
	char			specialization;
	char			wises;
	char			trait_tender, trait_leader;
};
