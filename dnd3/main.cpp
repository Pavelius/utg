#include "creature.h"
#include "calculator.h"
#include "draw.h"
#include "draw_strategy.h"
#include "numberlist.h"
#include "room.h"

void initialize_ui();
void update_ui();

static void main_scene() {
}

static void add_creature(point index, const char* id) {
	player->create(id);
	player->index = index;
}

static void add_map(point index, const char* avatar) {
	auto pr = bsdata<roomi>::add();
	pr->setavatar(avatar);
}

static void test_creature() {
	add_creature({12, 10}, "Skeleton");
	add_map({0, 0}, "hills");
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
	numberlist::read("rules/shdwdark/Tables.txt");
	return draw::strategy(start_main, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}