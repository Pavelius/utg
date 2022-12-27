#include "answers.h"
#include "actable.h"
#include "bsreq.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "creature.h"
#include "groupname.h"
#include "main.h"
#include "pushvalue.h"

static void set_mouse_guard(int count) {
	static char temp[260];
	player = bsdata<hero>::add();
	stringbuilder sb(temp); sb.add("%MouseGuard #%1i", count);
	answers::header = temp;
}

static void create_party() {
	pushvalue push_interactive(answers::interactive);
	answers::interactive = true;
	set_mouse_guard(1);
	player->create();
	player->add(Fighter, 2);
	party.add(player);
	set_mouse_guard(2);
	player->create();
	player->add(Fighter, 3);
	party.add(player);
}

static void starting() {
	create_party();
	player->roll(Fighter);
}

static void initialize() {
	groupname::read("locale/ru/Mouseguards.txt");
	bsreq::read("rules/Wise.txt");
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