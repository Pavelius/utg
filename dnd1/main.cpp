#include "bsref.h"
#include "bsreq.h"
#include "creature.h"
#include "gender.h"
#include "scenery.h"
#include "draw_object.h"
#include "draw_utg.h"
#include "log.h"
#include "questlist.h"
#include "rand.h"
#include "scenery.h"
#include "script.h"
#include "speech.h"

void combat_mode();
void initialize_ui();
void random_encounter(const char* id);

static void add_creature(const char* id, gendern gender, featn feat, int level) {
	auto pi = bsdata<classi>::find(id);
	if(!pi)
		return;
	add_creature(pi, gender, level);
	player->set(feat);
}

static void starting() {
	add_scene("Forest");
	add_creature("Fighter", Male, Player, 1);
	add_creature("Fighter", Male, Player, 1);
	add_creature("Cleric", Male, Player, 2);
	add_creature("Halfling", Female, Player, 1);
	creatures.select();
	start_game_organization();
	start_game_reputation();
	answers::header = "Большой зал";
	random_encounter("Orc");
	script_run("CombatMode");
	//scene->enter();
}

void initialize_str();

static void initializing() {
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
	draw::heroes_fade = get_fade;
	//srand(getcputime());
	srand(47167);
	answers::resid = "meet";
	initialize_ui();
	return utg::start(starting, initializing);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}