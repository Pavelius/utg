#include "answers.h"
#include "actable.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "creature.h"
#include "main.h"

static void starting() {
	hero player;
	answers an;
	player.roll(Fighter);
}

static void initialize() {
}

int	main(int argc, char *argv[]) {
	answers::console = &sb;
	answers::prompt = sb.begin();
	answers::resid = "treasure";
	draw::object::initialize();
	return draw::start(starting, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}