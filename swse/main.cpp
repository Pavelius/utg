#include "ability.h"
#include "area.h"
#include "bsreq.h"
#include "creature.h"
#include "draw_utg.h"

void one_combat_round();
void initialize_printer();

static void initialize_answers() {
	static char console_text[2048];
	static stringbuilder console(console_text);
	answers::resid = "start";
	answers::console = &console;
}

static void initialize() {
	initialize_answers();
	initialize_printer();
}

static bool test_area() {
	area a1 = {};
	auto n = a1.geti().id;
	return n != 0;
}

static void generate_character() {
	add_area("Hangar");
	create_hero(Jedi, Male);
	add_creatures();
	add_item("Quarterstaff");
	player->setability(Relation, -100);
	create_hero(Scoundrell, Female);
	add_item("Pistol");
	add_creatures();
	one_combat_round();
}

int main(int argc, char* argv[]) {
	srand(getcputime());
	initialize();
	return draw::start(generate_character);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}