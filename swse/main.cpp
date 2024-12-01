#include "ability.h"
#include "area.h"
#include "bsreq.h"
#include "creature.h"
#include "draw_utg.h"
#include "timer.h"
#include "rand.h"
#include "script.h"
#include "stringvar.h"

void act_custom(stringbuilder& sb, const char* identifier);

static void main_custom(stringbuilder& sb, const char* identifier) {
	if(stringvar_identifier(sb, identifier))
		return;
	act_custom(sb, identifier);
}

static void initialize_printer() {
	stringbuilder::custom = main_custom;
}

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