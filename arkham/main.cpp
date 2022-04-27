#include "bsreq.h"
#include "main.h"
#include "utg.h"

void main_menu();

static void initialization() {
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::start(main_menu, true, initialization);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}