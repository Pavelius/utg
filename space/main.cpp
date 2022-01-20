#include "bsreq.h"
#include "draw.h"
#include "log.h"
#include "main.h"

static void start_mission() {
}

static void test_game() {
	mission.clear();
	mission.add("Marines");
	mission.add("Marines");
	answers an;
	an.add(start_mission, "Начать миссию");
	logs::choose(an, "Что делать дальше?", 0);
}

int main(int argc, char* argv[]) {
	logs::url = "EliteTroops";
	return draw::utg::run(test_game, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}