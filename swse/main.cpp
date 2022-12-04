#include "ability.h"
#include "bsreq.h"
#include "creature.h"
#include "utg.h"

void initialize_script();

static void initialize() {
	initialize_script();
	bsreq::read("rules/Advance.txt");
}

static void generate_character() {
	creature player;
	answers an;
	player.clear();
	player.create(bsdata<classi>::find("Jedi"), Male);
	an.add(0, "Test");
	an.choose();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	answers::resid = "meet";
	return draw::start(generate_character, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}