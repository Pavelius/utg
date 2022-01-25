#include "main.h"

static creature* create_hero(bool interactive) {
	auto push_interactive = utg::interactive;
	utg::interactive = interactive;
	auto p = bsdata<creature>::add();
	p->generate();
	utg::interactive = push_interactive;
	return p;
}

static void generate_character() {
	quest::read("rules/Quest.txt");
	charname::read("locale/ru/CharacterNames.txt");
	utg::header = getnm("CharacterGeneration");
	create_hero(false);
	create_hero(false);
	create_hero(false);
	create_hero(false);
	quest::run(1, utg::url, utg::header);
}

int main(int argc, char* argv[]) {
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = creature::getavatarst;
	utg::url = "plain_village";
	srand(getcputime());
	return draw::utgx::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}