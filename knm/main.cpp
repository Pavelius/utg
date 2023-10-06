#include "crt.h"
#include "draw_strategy.h"
#include "list.h"
#include "province.h"
#include "player.h"
#include "script.h"

void initialize_ui();
void initialize_script();

static void run_function(const char* id) {
	auto push_list = last_list;
	last_list = bsdata<listi>::find(id);
	if(last_list)
		script_run(last_list->elements);
	last_list = push_list;
}

static void start_game() {
	prepare_game_ui();
	province = bsdata<provincei>::elements + 0;
	run_function("GameStart");
}

static void initialize() {
	initialize_province();
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	srand(505);
	initialize_ui();
	initialize_script();
	return draw::strategy(start_game, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}