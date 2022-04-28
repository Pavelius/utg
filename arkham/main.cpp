#include "bsreq.h"
#include "main.h"
#include "strategy.h"

void main_menu();

static void initialization() {
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::strategy(main_menu, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}