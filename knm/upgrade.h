#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct listi;

struct upgradei : entity, abilitya {
	listi*		trigger;
	variants	effect;
};