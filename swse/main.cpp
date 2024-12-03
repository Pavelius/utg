#include "ability.h"
#include "area.h"
#include "bsreq.h"
#include "creature.h"
#include "draw_utg.h"
#include "timer.h"
#include "rand.h"
#include "script.h"
#include "stringvar.h"

void initialize_printer();

static void initialize_answers() {
	static char console_text[2048];
	static stringbuilder console(console_text);
	answers::resid = "start";
	answers::console = &console;
}

static void initialize_code() {
	script_run("MainScript");
}

static void initialize() {
	initialize_answers();
	initialize_printer();
	initialize_code();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return utg::start(0, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}