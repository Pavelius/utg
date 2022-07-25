#include "draw.h"
#include "draw_object.h"
#include "main.h"
#include "strategy.h"

using namespace draw;

static void main_scene() {
	statable t1 = {};
	statable t2 = {0, 0, 2, 1};
	statable t3 = t1;
	t3 += t2;
}

static void start_game() {
	draw::scene(main_scene);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}