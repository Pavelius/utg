#include "point.h"

#pragma once

namespace draw {
int			getdistance(point h1, point h2);
point		fh2p(point hex, int size);
point		fp2h(point pt, int size);
point		h2p(point hex, int size);
void		hexagon();
void		fhexagon();
void		hexagonf();
point		p2h(point pt, int size);
}
