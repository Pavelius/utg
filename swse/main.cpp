#include "ability.h"
#include "utg.h"

static void initialize() {
}

static void generate_character() {
	answers an;
	an.add(0, "Test");
	an.choose();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	answers::resid = "meet";
	return draw::start(generate_character, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}