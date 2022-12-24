#include "flagable.h"

#pragma once

enum tag_s : unsigned char {
	Intimate, Close, Far,
};
struct tagi {
	const char* id;
};
typedef flagable<8> taga;
