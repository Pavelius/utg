#include "bsreq.h"
#include "code_package.h"
#include "draw.h"
#include "log.h"
#include "main.h"
#include "quest.h"
#include "script.h"

static void start_mission() {
}

void test_parser();

void test_package() {
	code::package e;
	e.create("io.file");
	e.write("packages/io/file.pkg");
}

static void test_game() {
	test_parser();
	test_package();
	quest::run(1000);
}

static void reading() {
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