#include "rand.h"
#include "timer.h"
#include "draw_strategy.h"

static void initialization() {
}

static void main_menu() {
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return 0;
	// return draw::strategy(main_menu, initialization);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}