#include "answers.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "main.h"

static void starting() {
	answers an;
	an.add("Test", "test");
	an.choose("Test");
}

static void initialize() {
}

int	main(int argc, char *argv[]) {
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	answers::resid = "meet";
	draw::object::initialize();
	return draw::start(starting, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}