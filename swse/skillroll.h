#include "nameable.h"
#include "feat.h"
#include "skill.h"
#include "variant.h"

#pragma once

struct skillroll : nameable {
	skill_s		skill;
	variants	required, bonuses;
	featf		reroll;
	char		dc;
};