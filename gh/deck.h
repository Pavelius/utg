#include "adat.h"
#include "array.h"
#include "rand.h"

#pragma once

struct deck : adat<short unsigned> {
	short unsigned	choose(const char* title, const array& source, bool need_remove);
	void			discard(short unsigned v);
	void			shuffle() { zshuffle(data, count); }
	short unsigned	take();
};