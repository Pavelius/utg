#include "creature.h"
#include "draw.h"
#include "draw_strategy.h"
#include "numberlist.h"
#include "room.h"

void initialize_ui();
void update_ui();

static void main_scene() {
}

static void add_creature(point index, const char* avatar) {
	player = bsdata<creature>::add();;
	player->create();
	player->setavatar(avatar);
	player->index = index;
	player->update();
}

static void test_creature() {
	add_creature({12, 10}, "me1");
	auto pr = bsdata<roomi>::add();
	pr->setavatar("hills");
	update_ui();
}

static void start_main() {
	test_creature();
	draw::scene(main_scene);
}

static void initialize() {
	initialize_ui();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	numberlist::read("rules/Tables.txt");
	return draw::strategy(start_main, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}