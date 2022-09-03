#include "main.h"

gamei game;
static char sb_console_text[4096];
static stringbuilder sb_console(sb_console_text);

static bool rollsurprisedice(int value = 2) {
	return d6() >= value;
}

void gamei::initialize() {
	answers::console = &sb_console;
}

void gamei::rollsurprise() {
	party_surprised = rollsurprisedice(0);
	monster_surprised = rollsurprisedice(0);
}

void gamei::rollreaction(int bonus) {
	auto r = d6() + d6() + bonus;
	if(r <= 2)
		reaction = Hostile;
	else if(r <= 5)
		reaction = Unfriendly;
	else if(r <= 8)
		reaction = Unfriendly;
	else if(r <= 11)
		reaction = Unfriendly;
	else
		reaction = Friendly;
}

void gamei::mainmenu() {
}