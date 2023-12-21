#include "bsref.h"
#include "bsreq.h"
#include "creature.h"
#include "gender.h"
#include "scenery.h"
#include "stringlist.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "generator.h"

void combat_mode();
void random_encounter(const char* id);
void spell_initialize();

static void add_creature(const char* id, gender_s gender, feat_s feat, int level) {
	auto pi = bsdata<classi>::find(id);
	if(!pi)
		return;
	add_creature(pi, gender, level);
	player->set(feat);
}

static void starting() {
	scene = bsdata<scenery>::add();
	add_creature("Fighter", Male, Player, 1);
	add_creature("Fighter", Male, Player, 1);
	add_creature("Cleric", Male, Player, 2);
	add_creature("Halfling", Female, Player, 1);
	creatures.select();
	start_game_organization();
	start_game_reputation();
	answers::header = "Большой зал";
	random_encounter("Orc");
	creatures.select();
	//treasure loot;
	//loot.generate('A');
	//loot.take();
	combat_mode();
}

void initialize_str();
void initialize_ui();

static void initializing() {
	stringlist::read("locale/ru/NameCharacters.txt");
	generatori::read("rules/RandomGems.txt");
	spell_initialize();
	initialize_str();
}

static const char* getavatarst(const void* p) {
	if(!(*((creature*)p)))
		return 0;
	return ((creature*)p)->avatarable::getavatar();
}

static bool isplayer(const void* p) {
	return p == player;
}

static int get_fade(const void* object) {
	auto p = (creature*)object;
	auto hp = p->get(HP);
	auto hpm = p->get(HPMax);
	return hp * 100 / hpm;
}

int main(int argc, char* argv[]) {
	auto i = bsdata<spelli>::source.getcount();
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = getavatarst;
	draw::heroes_isplayer = isplayer;
	draw::heroes_getfade = get_fade;
	srand(getcputime());
	//srand(1123);
	answers::resid = "meet";
	initialize_ui();
	return draw::start(starting, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}