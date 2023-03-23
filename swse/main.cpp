#include "ability.h"
#include "bsreq.h"
#include "creature.h"
#include "draw_utg.h"

static void initialize() {
	bsreq::read("rules/Feats.txt");
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Advance.txt");
	bsreq::read("rules/SkillRoll.txt");
}

static void generate_character() {
	creature player;
	answers an;
	player.create(Jedi, Male);
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