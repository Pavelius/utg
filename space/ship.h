#include "item.h"
#include "draw_posable.h"

#pragma once

struct shipi : item {
};
struct ship : posable {
	unsigned char	type;
	void			paint() const;
};
