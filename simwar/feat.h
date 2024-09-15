#pragma once

#include "nameable.h"

enum featn : unsigned char {
	Building, Water,
	Discard, Shuffle, Extend,
};
struct feati : nameable {
};
struct featable {
	unsigned	feat;
	bool		is(featn v) const { return (feat & (1 << v)) != 0; }
};
