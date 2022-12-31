#include "answers.h"
#include "bsreq.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "hero.h"
#include "groupname.h"
#include "pushvalue.h"
#include "questlist.h"

static void set_mouse_guard(int count) {
	static char temp[260];
	player = bsdata<hero>::add();
	stringbuilder sb(temp); sb.add("%MouseGuard #%1i", count);
	answers::header = temp;
}

static void create_party() {
	answers::interactive = false;
	set_mouse_guard(1);
	player->create();
	party.add(player);
	set_mouse_guard(2);
	player->create();
	party.add(player);
	answers::interactive = true;
}

static void starting() {
	create_party();
	player->roll(Fighter);
}

static void initialize() {
	groupname::read("locale/ru/Mouseguards.txt");
	bsreq::read("rules/Wise.txt");
	bsreq::read("rules/Traits.txt");
	quest::initialize();
	questlist::read("NatureQuest");
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