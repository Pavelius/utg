#include "main.h"

void initialize_png();

static void right_panel() {
	draw::propertybar();
}

static void generate_character() {

}

int main(int argc, char* argv[]) {
	initialize_png();
	answers::beforepaint = right_panel;
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}