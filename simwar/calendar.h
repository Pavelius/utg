#include "stringbuilder.h"

#pragma once

struct calendari {
	unsigned		year, turn;
	void			getname(stringbuilder& sb) const;
	int				getyear() const { return year + turn / 12; }
};
