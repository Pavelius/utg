#include "nameable.h"
#include "feat.h"
#include "skill.h"
#include "variant.h"

#pragma once

struct skillroll : nameable {
	skilln		skill;
	variants	required, bonuses;
	featf		reroll;
	char		dc;
};