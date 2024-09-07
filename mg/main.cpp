#include "answers.h"
#include "bsreq.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "hero.h"
#include "log.h"
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
	player->setavatar("guard1");
	party.add(player);
	set_mouse_guard(2);
	player->create();
	player->setavatar("guard2");
	player->skills_pass[Fighter]++;
	player->skills_pass[Fighter]++;
	player->skills_fail[Fighter]++;
	party.add(player);
	answers::interactive = true;
}

static void starting() {
	create_party();
	player->roll(Fighter);
}

static void initialize() {
	read_group_names("names/ru/Mouseguards.txt");
	bsreq::read("rules/Wise.txt");
	bsreq::read("rules/Traits.txt");
	log::readlf(questlist_read, "quest", "*.txt");
	quest_initialize();
}

static const char* getavatarst(const void* p) {
	return ((hero*)p)->getavatar();
}

static bool isplayer(const void* p) {
	return player == p;
}

int	main(int argc, char *argv[]) {
	draw::heroes = bsdata<hero>::source_ptr;
	draw::heroes_getavatar = getavatarst;
	draw::heroes_isplayer = isplayer;
	answers::console = &sb;
	answers::prompt = sb.begin();
	answers::resid = "hamlet";
	return utg::start(starting, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}