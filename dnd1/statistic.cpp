#include "statistic.h"

statistici* statistica::findadd(void* object) {
	for(auto p : *this) {
		if(p->object == object)
			return p;
	}
	auto np = (statistici*)adat::add();
	np->object = object;
	np->count = 0;
	return np;
}

void statistica::add(void* object, int count) {
	findadd(object)->count += count;
}