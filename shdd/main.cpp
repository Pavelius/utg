#include "answers.h"
#include "bsreq.h"
#include "creature.h"
#include "crt.h"
#include "draw.h"
#include "draw_utg.h"
#include "gender.h"
#include "groupname.h"
#include "item.h"
#include "log.h"
#include "questlist.h"
#include "script.h"

void stringbuilder_proc(stringbuilder& sb, const char* id);
void initialize_ui();

static void starting() {
	creature::add("Orc");
	creature::add(Dwarf, Male, Fighter);
	creature::add(Human, Male, Fighter);
	auto pn = bsdata<listi>::find("MainScript");
	if(!pn)
		return;
	script::run(pn->elements);
}

static void initialize() {
	log::readloc(groupname::read, "Players");
}

static const char* getavatarst(const void* p) {
	if(!(*((creature*)p)))
		return 0;
	return ((creature*)p)->avatarable::getavatar();
}

static bool isplayer(const void* p) {
	return p == player;
}

static void initialize_avatars() {
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = getavatarst;
	draw::heroes_isplayer = isplayer;
}

int main(int argc, char* argv[]) {
	initialize_ui();
	quest::initialize();
	initialize_avatars();
	srand(getcputime());
	//srand(1123);
	stringbuilder::custom = stringbuilder_proc;
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	return draw::start(starting, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}