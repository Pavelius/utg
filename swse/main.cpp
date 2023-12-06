#include "ability.h"
#include "bsreq.h"
#include "creature.h"
#include "draw_utg.h"

static void initialize() {
}

static void generate_character() {
	creature player;
	player.create(Jedi, Male);
	an.clear();
	an.add(0, "Test");
	an.choose();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	answers::resid = "start";
	return draw::start(generate_character, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}