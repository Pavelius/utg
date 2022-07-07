#include "bsreq.h"
#include "strategy.h"
#include "main.h"

void status_info(void) {
}

static void main_menu() {
}

static void initialize() {
	bsreq::read("rules/DwarvenRaces.txt");
}

int main(int argc, char* argv[]) {
	//srand(getcputime());
	return draw::strategy(main_menu, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}