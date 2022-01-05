#include "main.h"

static void generate_character() {
	logs::header = getnm("CharacterGeneration");
	logs::url = "meet";
	logs::interactive = true;
	creature e;
	e.generate();
}

int main(int argc, char* argv[]) {
	return draw::utg::run(generate_character);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}