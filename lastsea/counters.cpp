#include "crt.h"
#include "counters.h"
#include "stringbuilder.h"

void counters::clear() {
	memset(this, 0, sizeof(*this));
}

void counters::set(int v, int i) {
	if(v >= maxcount)
		return;
	elements[v].value = i;
}