#include "crt.h"

#pragma once

namespace metrics {
extern unsigned char opacity;
}
namespace draw {
void		strategy_background();
void		texth2w(const char* string);
bool		ishilite(int size, const void* object);
bool		ishilite(const void* object);
bool		swindow(bool hilight);
int			strategy(fnevent start, fnevent afterread, const char* rules_url = 0);
}
