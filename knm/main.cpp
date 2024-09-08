#include "area.h"
#include "deck.h"
#include "draw_strategy.h"
#include "entitya.h"
#include "list.h"
#include "province.h"
#include "player.h"
#include "rand.h"
#include "timer.h"

void prepare_game_ui();
void initialize_decks();
void initialize_ui();
void initialize_script();
void update_ui();

void game_var_identifier(stringbuilder& sb, const char* identifier);

#ifdef _DEBUG
void util_main();
#endif // _DEBUG

static void start_game() {
	area.clear();
	province = bsdata<provincei>::elements + 0;
	speaker = bsdata<playeri>::elements + 0;
	initialize_province();
	initialize_decks();
	prepare_game_ui();
	update_ui();
	run_command("GamePlay");
}

static void initialize() {
#ifdef _DEBUG
	util_main();
#endif // _DEBUG
	stringbuilder::custom = game_var_identifier;
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