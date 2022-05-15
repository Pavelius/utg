#include "crt.h"

#pragma once

typedef short unsigned cardt;
struct deck : adat<cardt> {
	void			shuffle() { zshuffle(data, count); }
	cardt			take();
};

