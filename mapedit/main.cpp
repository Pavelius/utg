#include "bsreq.h"
#include "draw.h"
#include "draw_strategy.h"
#include "rand.h"
#include "timer.h"

void read_tiles();
void ui_initialize();
void ui_start();
void util_main();

static void initialize() {
	read_tiles();
	ui_initialize();
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	srand(505);
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	ui_initialize();
	return draw::strategy(ui_start, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}