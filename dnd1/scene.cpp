#include "main.h"

creaturea scene::targets;

creature* scene::choosetarget() const {
	answers an;
	for(auto p : targets)
		an.add(p, p->getname());
	return (creature*)an.choose(getnm("ChooseTarget"));
}

static int compare_initiative(const void* v1, const void* v2) {
	auto p1 = *((creature**)v1);
	auto p2 = *((creature**)v2);
	return p1->initiative - p2->initiative;
}

void scene::rollinitiative() {
	for(auto p : creatures)
		p->initiative = d6();
	qsort(creatures.data, creatures.count, sizeof(creatures.data[0]), compare_initiative);
}