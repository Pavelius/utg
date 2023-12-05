#include "action.h"
#include "nameable.h"
#include "skill.h"

#pragma once

struct conflicti : nameable {
	skill_s			base, roll;
	skill_s			skills[Maneuver + 1], alternate[Maneuver + 1];
};
