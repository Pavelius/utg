#include "bsref.h"
#include "bsreq.h"
#include "creature.h"
#include "stringlist.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "generator.h"

void combat_mode();
void random_encounter(const char* id);

static creature* create_player(class_s type, gender_s gender, feat_s feat) {
	auto p = bsdata<creature>::add();
	p->create(type, gender);
	p->set(feat);
	return p;
}

static void starting() {
	auto p1 = create_player(Fighter, Male, Player);
	auto p2 = create_player(Fighter, Male, Player);
	answers::header = "Большой зал";
	random_encounter("Skeleton");
	creatures.select();
	//treasure loot;
	//loot.generate('A');
	//loot.take();
	combat_mode();
}

void initialize_str();
void initialize_ui();

static void initializing() {
	bsreq::read("rules/Items.txt");
	bsreq::read("rules/Monsters.txt");
	stringlist::read("locale/ru/NameCharacters.txt");
	generatori::read("rules/RandomGems.txt");
	initialize_str();
}

static const char* getavatarst(const void* p) {
	if(!(*((creature*)p)))
		return 0;
	return ((creature*)p)->avatarable::getavatar();
}

static bool isplayer(const void* p) {
	return p==player;
}

int main(int argc, char* argv[]) {
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = getavatarst;
	draw::heroes_isplayer = isplayer;
	srand(getcputime());
	//srand(1123);
	answers::resid = "meet";
	initialize_ui();
	return draw::start(starting, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}