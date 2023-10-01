#include "crt.h"
#include "draw.h"
#include "draw_strategy.h"

static void start_game() {

}

static void initialize() {
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	srand(505);
	return draw::strategy(start_game, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}