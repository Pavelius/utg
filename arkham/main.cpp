#include "bsreq.h"
#include "main.h"
#include "utg.h"

static void starting() {
}

static void initialization() {
	bsreq::read("rules/Items.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::start(starting, true, initialization);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}