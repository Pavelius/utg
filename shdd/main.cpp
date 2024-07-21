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
	add_creature("Orc");
	add_creature("Human", Male, Fighter);
	add_creature("Dwarf", Male, Fighter);
	auto pn = bsdata<listi>::find("MainScript");
	if(!pn)
		return;
	script_run(pn->elements);
}

static void initialize() {
	log::readloc(groupname::read, 0, "Players");
}

static const char* getavatarst(const void* p) {
	if(!(*((creature*)p)))
		return 0;
	return ((creature*)p)->avatarable::getavatar();
}

static bool isplayer(const void* p) {
	return p == player;
}

static void setplayer(void* p) {
	if(action_change_player)
		draw::breakmodal((long)p);
}

static void initialize_avatars() {
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = getavatarst;
	draw::heroes_isplayer = isplayer;
	draw::heroes_setplayer = setplayer;
}

int main(int argc, char* argv[]) {
	initialize_ui();
	quest_initialize();
	initialize_avatars();
	srand(getcputime());
	srand(8381);
	stringbuilder::custom = stringbuilder_proc;
	answers::console = &utg::sb;
	answers::prompt = utg::sb.begin();
	return draw::start(starting, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}