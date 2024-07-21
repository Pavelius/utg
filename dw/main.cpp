#include "creature.h"
#include "console.h"
#include "draw.h"
#include "pushvalue.h"
#include "questlist.h"
#include "script.h"

static char	console_text[4096];
static stringbuilder console(console_text);
bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gender_s gender);

static void create_hero() {
	player = bsdata<creature>::add();
	player->generate();
}

static void play_settlement() {
	last_questlist = bsdata<questlist>::elements;
	quest_run(1);
}

static void character_generation() {
	answers::header = getnm("CharacterGeneration");
	answers::resid = "meet";
	pushvalue push(answers::interactive, false);
	create_hero();
	create_hero();
	create_hero();
	create_hero();
	draw::setnext(play_settlement);
}

static void initialize() {
	quest_initialize();
	stringlist::read("locale/ru/other/CharacterNames.txt");
	questlist_read("rules/Quest.txt");
}

static void stringbuilder_custom(stringbuilder& sb, const char* id) {
	if(player) {
		if(apply_action(id, sb, player->getname(), player->getgender()))
			return;
	}
	stringbuilder::defidentifier(sb, id);
}

static void setplayer(void* p) {
	draw::breakmodal((long)p);
}

static bool isplayer(const void* p) {
	return p == player;
}

static void initialize_avatars() {
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = creature::getavatarst;
	draw::heroes_isplayer = isplayer;
	draw::heroes_setplayer = setplayer;
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	initialize_console();
	initialize_avatars();
	stringbuilder::custom = stringbuilder_custom;
	answers::console = &console;
	return draw::start(character_generation, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}