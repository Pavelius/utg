#include "crt.h"
#include "draw_strategy.h"
#include "list.h"
#include "script.h"

static void run_function(const char* id) {
	auto p = bsdata<listi>::find(id);
	if(!p)
		return;
	script_run(p->elements);
}

static void start_game() {
	run_function("GameTurn");
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