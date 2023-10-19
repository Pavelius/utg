#include "ability.h"
#include "entity.h"
#include "variant.h"

#pragma once

struct listi;
struct upgradei;

struct upgradei : entity, abilitya {
	const char*	trigger;
	variants	effect;
	unsigned	required;
};
struct upgradea {
	unsigned	upgrades;
	int			getupgradecount() const;
	bool		isupgrade(const upgradei* v) const;
	bool		isupgradeallow(const upgradei* v) const;
	void		setupgrade(const upgradei* v);
};
extern upgradei* last_upgrade;