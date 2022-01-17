#include "main.h"

static void right_panel() {
	draw::propertybar();
}

static void generate_character() {
	logs::interactive = true;
	logs::url = "northmen";
	game.clear();
	game.suffer.add(Injury, 2);
	game.inflict.add(Injury, 1);
	creature player = {};
	player.move(EngageMelee);
}

int main(int argc, char* argv[]) {
	srand(102);
	answers::beforepaint = right_panel;
	quest::read("rules/Moves.txt");
	messagei::read("rules/MoveMessages.txt");
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}