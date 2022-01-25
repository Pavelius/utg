#include "point.h"

#pragma once

struct sprite;
namespace draw {
struct object : point {
	void*			data;
	const sprite*	resource;
	unsigned short	frame;
	unsigned		flags;
	void			paint() const;
};
void				paintobjects();
}
