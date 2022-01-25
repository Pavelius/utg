#include "point.h"

#pragma once

namespace draw {
extern int				fsize;
int						getdistance(point h1, point h2);
point					h2p(point hex, int size);
void					hexagon();
void					hexagonf();
point					p2h(point pt, int size);
}
