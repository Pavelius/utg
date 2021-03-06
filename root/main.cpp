#include "main.h"

static void generate_character() {
	char temp[16]; stringbuilder sb(temp);
	//game.clear();
	//game.suffer.add(Injury, 2);
	//game.inflict.add(Injury, 1);
	creature player = {};
	player.id = "Umberto";
	player.abilities[Might] += 2;
	player.abilities[Cunning] -= 1;
	player.move(EngageMelee);
	//player.apply(game);
	draw::pause();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	quest::read("rules/Moves.txt");
	messagei::read("rules/MoveMessages.txt");
	answers::resid = "northmen";
	return draw::start(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}