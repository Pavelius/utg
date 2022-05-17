#include "main.h"

void creaturea::select() {
	auto ps = data;
	auto pe = endof();
	for(auto& e : bsdata<creaturei>()) {
		if(!e)
			continue;
		if(ps < pe)
			*ps++ = &e;
	}
	count = ps - data;
}

static int compare(const void* v1, const void* v2) {
	auto p1 = *((creaturei**)v1);
	auto p2 = *((creaturei**)v2);
	auto i1 = p1->getinitiative();
	auto i2 = p2->getinitiative();
	if(i1 != i2)
		return i1 - i2;
	i1 = p1->getinitiative(1);
	i2 = p2->getinitiative(1);
	if(i1 != i2)
		return i1 - i2;
	return 0;
}

void creaturea::sort() {
	qsort(data, count, sizeof(data[0]), compare);
}