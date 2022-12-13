#include "actable.h"
#include "group.h"

#pragma once

struct npc : actable {
	actable* leader;
	group_s	group;
	bool	isgroup() const { return group != Individual; }
	void	setgroup(group_s v);
};
extern npc* opponent;