#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct listi;

struct upgradei : entity, abilitya {
	const char*	trigger;
	variants	effect;
};
struct upgradea {
	unsigned	upgrades;
	bool		isupgrade(const upgradei* v) const;
	void		setupgrade(const upgradei* v);
};