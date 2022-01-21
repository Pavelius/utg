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
	logs::header = getnm("CharacterGeneration");
	create_hero(false);
	create_hero(false);
	create_hero(false);
	create_hero(true);
	logs::pause();
}

int main(int argc, char* argv[]) {
	draw::protogonists = bsdata<varianti>::elements + Creature;
	quest::read("rules/Quest.txt");
	logs::url = "meet";
	srand(getcputime());
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}