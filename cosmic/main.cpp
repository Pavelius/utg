#include "bsreq.h"
#include "console.h"
#include "draw_utg.h"
#include "log.h"
#include "quest.h"
#include "script.h"
#include "ship.h"

void initilize_script();
void run_current_quest();

static void starting() {
	script_list("StartScript");
	run_current_quest();
}

static void initialize() {
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	quest::initialize();
	initialize_console();
	initilize_script();
	return draw::start(starting, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}