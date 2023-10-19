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
	int			getupgrade(ability_s v) const;
	int			getupgradecount() const;
	bool		isupgrade(const upgradei* v) const;
	bool		isupgrade(int v) const { return (upgrades & (1 << v)) != 0; }
	bool		isupgradeallow(const upgradei* v) const;
	void		setupgrade(const upgradei* v);
};
extern upgradei* last_upgrade;