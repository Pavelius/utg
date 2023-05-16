#include "ability.h"
#include "draw.h"
#include "draw_strategy.h"
#include "numberlist.h"

static void main_scene() {
}

static void start_main() {
	draw::scene(main_scene);
}

static void initialize() {
	numberlist::read("rules/Tables.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::strategy(start_main, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}