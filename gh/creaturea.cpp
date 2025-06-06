#include "main.h"

const creaturea* creaturea::last;

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

void creaturea::match(staten v, bool keep) {
	auto ps = data;
	auto pe = end();
	for(auto p : *this) {
		if(p->is(v) != keep)
			continue;
		*ps++ = p;
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

static int compare_nearest(const void* v1, const void* v2) {
	auto p1 = *((creaturei**)v1);
	auto p2 = *((creaturei**)v2);
	auto d1 = pathfind::getmove(p1->getindex());
	auto d2 = pathfind::getmove(p2->getindex());
	if(d1 != d2)
		return d1 - d2;
	return compare(v1, v2);
}

void creaturea::sortbymove() {
	qsort(data, count, sizeof(data[0]), compare_nearest);
}

void creaturea::range(int v) {
	auto ps = data;
	auto pe = end();
	for(auto p : *this) {
		if(pathfind::getmove(p->getindex()) > v)
			continue;
		*ps++ = p;
	}
	count = ps - data;
}