#include "draw.h"
#include "drawobject.h"
#include "strategy.h"

using namespace draw;

static void main_scene() {

}

static void start_game() {
	draw::scene(main_scene);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	return draw::strategy(start_game, 0);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}