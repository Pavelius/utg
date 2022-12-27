#include "answers.h"
#include "actable.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "creature.h"
#include "groupname.h"
#include "main.h"

static void create_party() {
	auto p = bsdata<hero>::add();
	p->create();
	p->add(Fighter, 3);
	party.add(p);
	p = bsdata<hero>::add();
	p->create();
	p->add(Fighter, 3);
	party.add(p);
	player = p;
}

static void starting() {
	create_party();
	answers an;
	player->roll(Fighter);
}

static void initialize() {
	groupname::read("locale/ru/Mouseguards.txt");
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