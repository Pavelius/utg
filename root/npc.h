#include "person.h"

#pragma once

struct npc : person {
	person* leader;
	char	group;
	bool	isgroup() const { return group != 0; }
	void	setgroup(int v);
};
extern npc* opponent;