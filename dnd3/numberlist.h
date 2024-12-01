#include "nameable.h"

#pragma once

struct numberlist : nameable {
	slice<int>	elements;
	int			match(int value) const;
	static void	read(const char* url);
};
