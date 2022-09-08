#include "main.h"

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

void scene::encounter(const monsteri* pm) {
	if(!pm)
		return;
	auto count = pm->dungeon.roll();
	for(auto i = 0; i < count; i++) {
		auto p = bsdata<creature>::add();
		p->create(*pm);
		p->set(Enemy);
	}
	if(count > 1)
		draw::output(getnm("AppearSeveral"), pm->getname());
	else
		draw::output(getnm("AppearSingle"), pm->getname());
}