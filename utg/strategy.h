#include "crt.h"

#pragma once

namespace metrics {
extern unsigned char opacity;
}
namespace draw {
bool ishilite(int size, const void* object);
bool ishilite(const void* object);
int strategy(fnevent start, fnevent afterread);
bool swindow(bool hilight);
void texth2w(const char* string);
}
