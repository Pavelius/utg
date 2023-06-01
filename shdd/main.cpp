#include "answers.h"
#include "bsreq.h"
#include "creature.h"
#include "crt.h"
#include "draw.h"
#include "draw_utg.h"
#include "item.h"
#include "quest.h"

static void starting() {
	creature::add("Orc");
	creature::add(Dwarf, Fighter);
	an.clear();
	an.add(0, "Test case 1");
	an.add((void*)1, "Test case 2");
	an.choose("Choose case");
}

static void initialize() {
	quest::read("locale/ru/Actions.txt");
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	//srand(1123);
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	answers::resid = "meet";
	return draw::start(starting, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}