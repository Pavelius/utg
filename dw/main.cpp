#include "main.h"

static void test_a1() {
}

static void main_game() {
	answers an;
	an.add(test_a1, "Тестирование");
	an.choose("Что будете делать?", getnm("Cancel"), true, "meet", -1, getnm("CharacterGeneration"));
}

int main(int argc, char* argv[]) {
	return draw::utg::run(main_game);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}