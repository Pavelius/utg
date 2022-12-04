#include "advance.h"
#include "creature.h"
#include "script.h"

creature *player;
collection<creature> creatures;

static void advance_creature(int level, variant object) {
	for(auto& e : bsdata<advancei>()) {
		if(e.level == level && e.object == object)
			runscript(e.elements);
	}
}

void creature::clear() {
	memset(this, 0, sizeof(*this));
}

void creature::addclass(const classi* pv) {
	auto push_player = player; player = this;
	variant v = pv;
	advance_creature(classes[v.value], pv);
	classes[v.value]++;
	player = push_player;
}

void creature::create(const classi* pclass, gender_s gender) {
	setgender(gender);
	addclass(pclass);
}