#include "answers.h"
#include "creature.h"
#include "draw.h"
#include "draw_strategy.h"
#include "numberlist.h"
#include "room.h"

void initialize_abilitites();
void initialize_ui();
void initialize_str();
void update_ui();

static void main_scene() {
}

static void add_player(point index, bool interactive) {
	auto push = answers::interactive;
	answers::interactive = interactive;
	player->create();
	player->index = index;
	answers::interactive = push;
}

void add_creature(point index, const char* id) {
	player->create(id);
	player->index = index;
}

static void add_map(point index, const char* avatar) {
	auto pr = bsdata<roomi>::add();
	pr->setavatar(avatar);
}

static void test_creature() {
	//add_player({12, 10}, true);
	add_creature({12, 10}, "Skeleton");
	add_map({0, 0}, "forest_hh");
	update_ui();
}

static void start_main() {
	test_creature();
	draw::scene(main_scene);
}

static void initialize() {
	initialize_str();
	initialize_enumerators();
	initialize_abilitites();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	numberlist::read("rules/shdwdark/Tables.txt");
	initialize_ui();
	return draw::strategy(start_main, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}