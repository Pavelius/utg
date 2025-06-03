#include "counters.h"
#include "slice.h"

void counters::clear() {
	memset(this, 0, sizeof(*this));
}

void counters::set(int v, int i) {
	if(v >= maxcount)
		return;
	if(i < 0)
		i = 0;
	elements[v].value = i;
}