#include "ability.h"
#include "bsreq.h"
#include "console.h"
#include "draw_utg.h"
#include "log.h"
#include "quest.h"
#include "rand.h"
#include "script.h"
#include "ship.h"
#include "timer.h"
#include "variant.h"

void initilize_script();
void run_current_quest();

static void starting() {
	script_list("StartScript");
	run_current_quest();
}

static void initialize() {
}

static void get_property(const void* object, variant v, stringbuilder& sb) {
	if(v.iskind<modulei>()) {
		auto n = player->get((modulen)v.value);
		if(!n)
			return;
		sb.add("%1i", n);
	}
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	quest_initialize();
	initialize_console();
	initilize_script();
	utg::callback::getinfo = get_property;
	return utg::start(starting, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}