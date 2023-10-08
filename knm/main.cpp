#include "crt.h"
#include "deck.h"
#include "draw_strategy.h"
#include "entitya.h"
#include "list.h"
#include "province.h"
#include "player.h"

void initialize_ui();
void initialize_script();

#ifdef _DEBUG
void util_main();
#endif // _DEBUG

static void initialize_decks() {
	for(auto& e : bsdata<decki>())
		e.create();
}

static void start_game() {
	prepare_game_ui();
	province = bsdata<provincei>::elements + 0;
	speaker = bsdata<playeri>::elements + 0;
	initialize_decks();
	run_command("GamePlay");
}

static void initialize() {
	initialize_province();
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
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