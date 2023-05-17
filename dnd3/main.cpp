#include "creature.h"
#include "draw.h"
#include "draw_strategy.h"
#include "numberlist.h"

static void main_scene() {
}

static void test_creature() {
	creature player;
	player.create();
	player.update();
}

static void start_main() {
	test_creature();
	draw::scene(main_scene);
}

static void initialize() {
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	numberlist::read("rules/Tables.txt");
	return draw::strategy(start_main, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}