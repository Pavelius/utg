#include "main.h"

static void right_panel() {
	draw::propertybar();
}

static effectable game;

static void apply_effect(void* object) {
	auto p = (messagei*)object;
	for(auto v : p->conditions)
		game.apply(v);
}

static void make_move(const char* title, variant v, int choose_count) {
	answers an;
	for(auto& e : bsdata<messagei>()) {
		if(!e.match(v))
			continue;
		if(!e.value)
			continue;
		an.add(&e, e.text);
	}
	logs::apply(an, title, apply_effect, choose_count);
}

static void generate_character() {
	logs::interactive = true;
	logs::url = "northmen";
	game.clear();
	game.suffer.add(Injury, 2);
	game.inflict.add(Injury, 1);
	make_move("Что будете делать?", "EngageMelee", 2);
}

int main(int argc, char* argv[]) {
	answers::beforepaint = right_panel;
	quest::read("rules/Moves.txt");
	messagei::read("rules/MoveMessages.txt");
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}