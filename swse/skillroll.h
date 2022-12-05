#pragma once
#include "nameable.h"
#include "feat.h"
#include "skill.h"
#include "variant.h"

struct skillroll : nameable {
	skill_s		skill;
	variants	required, bonuses;
	featf		reroll;
	short		dc;
};