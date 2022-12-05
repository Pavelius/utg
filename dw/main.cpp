#include "main.h"

static creature* create_hero(bool interactive) {
	auto push_interactive = answers::interactive;
	answers::interactive = interactive;
	auto p = bsdata<creature>::add();
	p->generate();
	answers::interactive = push_interactive;
	return p;
}

static void generate_character() {
	quest::read("rules/Quest.txt");
	charname::read("locale/ru/CharacterNames.txt");
	answers::header = getnm("CharacterGeneration");
	answers::resid = "meet";
	create_hero(true);
	create_hero(true);
	create_hero(true);
	create_hero(true);
}

int main(int argc, char* argv[]) {
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = creature::getavatarst;
	srand(getcputime());
	return draw::start(generate_character);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}