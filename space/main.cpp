#include "bsreq.h"
#include "code_package.h"
#include "draw.h"
#include "log.h"
#include "main.h"
#include "quest.h"
#include "script.h"
#include "strategy.h"

void status_info(void) {
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

static void initialize() {
	quest::read("rules/Quest.txt");
}

int main(int argc, char* argv[]) {
	quest::initialize();
	return draw::strategy(test_game, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}