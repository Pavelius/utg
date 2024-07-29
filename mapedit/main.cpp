#include "crt.h"
#include "draw_strategy.h"

void ui_initialize();

static void start_game() {
}

static void initialize() {
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	srand(505);
	ui_initialize();
	return draw::strategy(start_game, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}