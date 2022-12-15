#include "ability.h"

#pragma once

class gamei {
	char	bolster;
public:
	int		getaverage(ability_s v) const;
	int		getbolster() const { return bolster; }
	void	usebolster() { bolster--; }
	int		getdiscount() const { return getaverage(Charisma); }
};
extern gamei game;
