#include "item.h"
#include "drawable.h"

#pragma once

struct shipi : itemi {
};
struct ship : drawable {
	unsigned char	type;
	static ship*	add(const char* id, point position);
	void			paint() const;
};
