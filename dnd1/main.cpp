#include "draw_object.h"
#include "main.h"

static void starting() {
	answers an;
	an.add(0, getnm("Continue"));
	an.choose("Test");
}

static void initializing() {
}

int main(int argc, char* argv[]) {
	//utg::callback::getinfo = game.sfgetproperty;
	//utg::callback::getstatus = game.sfgetstatus;
	srand(getcputime());
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	draw::object::initialize();
	//goal::info = game.information;
	return draw::start(starting, true, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}