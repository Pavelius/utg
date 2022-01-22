#include "main.h"

static creature* create_hero(bool interactive) {
	auto push_interactive = logs::interactive;
	logs::interactive = interactive;
	auto p = bsdata<creature>::add();
	p->generate();
	logs::interactive = push_interactive;
	return p;
}

static void generate_character() {
	quest::read("rules/Quest.txt");
	charname::read("locale/ru/CharacterNames.txt");
	logs::header = getnm("CharacterGeneration");
	create_hero(false);
	create_hero(false);
	create_hero(false);
	create_hero(false);
	quest::run(1, "Что будете делать?", logs::url, logs::header);
}

int main(int argc, char* argv[]) {
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = creature::getavatarst;
	logs::url = "plain_village";
	srand(getcputime());
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}