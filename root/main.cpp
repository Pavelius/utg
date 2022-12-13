#include "answers.h"
#include "quest.h"
#include "vagabond.h"
#include "utg.h"

static void generate_character() {
	char temp[16]; stringbuilder sb(temp);
	//game.clear();
	//game.suffer.add(Injury, 2);
	//game.inflict.add(Injury, 1);
	player = bsdata<vagabond>::add();
	player->clear();
	player->id = "Umberto";
	player->abilities[Might] += 2;
	player->abilities[Cunning] -= 1;
	player->move(EngageMelee);
}

static void initialize() {
	quest::read("rules/Moves.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	//messagei::read("rules/MoveMessages.txt");
	answers::resid = "northmen";
	answers::console = &console;
	answers::prompt = console.begin();
	return draw::start(generate_character, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}