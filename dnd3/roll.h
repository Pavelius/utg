#pragma once

#include "nameable.h"

class stringbuilder;

struct rolli : nameable {
	char	c, d, p, m;
	char	b, w;
	int		roll() const;
	void	add(stringbuilder& sb) const;
};
extern char roll_result[20];
