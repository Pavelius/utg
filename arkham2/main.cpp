#include "draw_board.h"
#include "rand.h"
#include "timer.h"
#include "view.h"

int main(int argc, char* argv[]) {
	srand(getcputime());
	return board_game_run(main_menu);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}