#include "bsreq.h"
#include "draw.h"
#include "log.h"
#include "main.h"

static void start_mission() {
}

static void test_game() {
	game.clear();
	game.add("Marines");
	game.add("Marines");
	game.add("Raiders", true);
	game.getally().move(EngageMelee);
}

int main(int argc, char* argv[]) {
	logs::url = "EliteTroops";
	bsreq::read("rules/Basic.txt");
	messagei::read("rules/Quest.txt");
	return draw::utg::run(test_game, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}