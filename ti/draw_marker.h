#include "color.h"
#include "draw_figure.h"

#pragma once

namespace draw {
struct marker {
	figure		shape;
	int			size;
	color		fore;
	void*		data;
	void		paint() const;
};
}
