#include "ability.h"
#include "bsreq.h"
#include "creature.h"
#include "draw_utg.h"

void one_combat_round();

static void initialize() {
}

static void generate_character() {
	create_hero(Jedi, Male);
	add_creatures();
	add_item("Quarterstaff");
	player->setability(Relation, -100);
	create_hero(Scoundrell, Female);
	add_item("Pistol");
	add_creatures();
	one_combat_round();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	answers::resid = "start";
	return draw::start(generate_character, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}