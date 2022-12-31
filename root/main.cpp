#include "answers.h"
#include "draw_utg.h"
#include "quest.h"
#include "vagabond.h"

static void generate_character() {
	//game.clear();
	//game.suffer.add(Injury, 2);
	//game.inflict.add(Injury, 1);
	player = bsdata<vagabond>::add();
	player->clear();
	player->id = "Umberto";
	player->gender = Male;
	player->abilities[Might] += 2;
	player->abilities[Cunning] -= 1;
	player->move(GrappleEnemy);
}

static void initialize() {
	movei::read("rules/MoveConcept.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	answers::resid = "northmen";
	answers::console = &console;
	answers::prompt = console.begin();
	return draw::start(generate_character, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}