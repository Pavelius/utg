#include "bsreq.h"
#include "draw.h"
#include "log.h"
#include "main.h"
#include "quest.h"
#include "script.h"

static void start_mission() {
}

static void test_game() {
	quest::run(1000);
}

static void reading() {
	bsreq::read("rules/Basic.txt");
	quest::read("rules/Quest.txt");
}

int main(int argc, char* argv[]) {
	answers::prompt = utg::sb.begin();
	answers::console = &utg::sb;
	answers::resid = "EliteTroops";
	quest::initialize();
	return draw::start(test_game, true, reading);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}