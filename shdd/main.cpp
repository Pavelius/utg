#include "answers.h"
#include "bsreq.h"
#include "creature.h"
#include "crt.h"
#include "draw.h"
#include "draw_utg.h"
#include "item.h"
#include "questlist.h"
#include "script.h"

static void starting() {
	creature::add("Orc");
	creature::add(Dwarf, Fighter);
	auto pn = bsdata<listi>::find("MainScript");
	if(!pn)
		return;
	script::run(pn->elements);
}

static void initialize() {
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