#include "main.h"

static char		console[512];
stringbuilder	sb(console);
gamei			game;
cardpool		cards;
quests			gamei::quest_other;

int gamei::d6() {
	return 1 + rand() % 6;
}

void gamei::applyresult(void* result) {
	an.clear();
	if(!result)
		return;
	else if(bsdata<quest>::have(result)) {
		auto p = (quest*)result;
		game.apply(p->tags);
	} else
		((fnevent)result)();
}

void gamei::turn() {
	game.phase_movement();
	game.phase_encounter_arkham();
	game.phase_encounter_other();
}

void gamei::play() {
	while(true)
		turn();
}