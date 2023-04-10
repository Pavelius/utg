#include "item.h"
#include "drawable.h"

#pragma once

struct shipi : item {
};
struct ship : drawable {
	unsigned char	type;
	void			paint() const;
};
