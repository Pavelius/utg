#include "main.h"

static void generate_character() {
	logs::header = getnm("CharacterGeneration");
	logs::url = "meet";
	auto vclass = logs::choosei("Class", getnm("Cancel"));
	auto vrace = logs::choosei("Race", getnm("Cancel"));
}

int main(int argc, char* argv[]) {
	return draw::utg::run(generate_character);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}