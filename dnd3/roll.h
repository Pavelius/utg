#pragma once

#include "nameable.h"

struct rolli : nameable {
	char	c, d, p, m;
	char	b, w;
	int		roll() const;
};
extern char roll_result[20];
