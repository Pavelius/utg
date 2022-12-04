#include "nameable.h"
#include "flagable.h"

#pragma once

typedef flagable<8> featable;

enum feat_s : unsigned char {
	TwoAttacks
};

struct feati : nameable {
};
