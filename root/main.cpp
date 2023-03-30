#include "answers.h"
#include "draw_utg.h"
#include "quest.h"
#include "stringact.h"
#include "vagabond.h"

void initialize_str();

static void generate_character() {
	player = bsdata<vagabond>::add();
	player->clear();
	player->id = "Umberto";
	player->gender = Male;
	player->abilities[Might] += 2;
	player->abilities[Cunning] -= 1;
	player->move(GrappleEnemy);
}

static void initialize() {
	initialize_str();
	movei::read("rules/MoveConcept.txt");
	act_enemy_name = getnm("Fox");
	act_enemy_gender = Male;
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