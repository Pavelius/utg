#include "flagable.h"
#include "nameable.h"

#pragma once

enum tag_s : unsigned char {
	Independed, Water,
	Walking, Flying,
};
typedef flagable<4> tagf;
struct tagi : nameable {
};
