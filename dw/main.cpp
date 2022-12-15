#include "main.h"
#include "pushvalue.h"

static char console_text[4096];
static stringbuilder console(console_text);

static creature* create_hero() {
	auto p = bsdata<creature>::add();
	p->generate();
	return p;
}

static void play_settlement() {
	pushvalue push(answers::header, getnm("Settlement"));
	quest::manual(1);
}

static void character_generation() {
	answers::header = getnm("CharacterGeneration");
	answers::resid = "meet";
	answers::interactive = false;
	create_hero();
	create_hero();
	create_hero();
	create_hero();
	answers::interactive = true;
	draw::setnext(play_settlement);
}

static void initialize() {
	quest::initialize();
	charname::read("locale/ru/CharacterNames.txt");
	quest::read("rules/Quest.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = creature::getavatarst;
	answers::console = &console;
	return draw::start(character_generation, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}