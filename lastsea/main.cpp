#include "charname.h"
#include "main.h"

static void generate_pirate() {
	auto p = bsdata<pirate>::add();
	p->clear();
	p->randomname();
}

static void initialize_game() {
	bsdata<pirate>::source.clear();
	generate_pirate();
	generate_pirate();
	generate_pirate();
	generate_pirate();
	for(auto& e : bsdata<pirate>())
		e.generate();
	logs::pause();
}

int main(int argc, char* argv[]) {
	draw::heroes = bsdata<pirate>::source_ptr;
	draw::heroes_getavatar = pirate::getavatarst;
	logs::url = "pirate_kingship";
	srand(getcputime());
	charname::read("locale/ru/CharacterNames.txt");
	return draw::utg::run(initialize_game, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}