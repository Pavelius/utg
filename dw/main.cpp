#include "creature.h"
#include "console.h"
#include "pushvalue.h"
#include "quest.h"

static char	console_text[4096];
static stringbuilder console(console_text);
bool apply_action(const char* identifier, stringbuilder& sb, const char* name, gender_s gender);

static void create_hero() {
	player = bsdata<creature>::add();
	player->generate();
}

static void play_settlement() {
	pushvalue push(answers::header, getnm("Settlement"));
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
	quest_read("rules/Quest.txt");
}

static void stringbuilder_custom(stringbuilder& sb, const char* id) {
	if(player) {
		if(apply_action(id, sb, player->getname(), player->getgender()))
			return;
	}
	stringbuilder::defidentifier(sb, id);
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	initialize_console();
	stringbuilder::custom = stringbuilder_custom;
	draw::heroes = bsdata<creature>::source_ptr;
	draw::heroes_getavatar = creature::getavatarst;
	answers::console = &console;
	return draw::start(character_generation, initialize);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}