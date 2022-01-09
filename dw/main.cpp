#include "main.h"

void initialize_png();

static void statusinfo(const void* object, stringbuilder& sb) {
	if(bsdata<creature>::source.have(object))
		((creature*)object)->getinfo(sb);
}

static void party_information() {
	for(auto i = 0; i < 4; i++) {
		auto& e = bsdata<creature>::elements[i];
		if(e)
			draw::avatar(e.getavatar(), &e);
		else
			draw::noavatar();
	}
}

static void tabs_panel() {
	if(bsdata<creature>::source.have(draw::focus_object)) {
		auto pm = bsdata<menu>::elements;
		auto push_title = draw::title_width;
		draw::title_width = 120;
		draw::label(draw::focus_object, pm->elements, creature::getinfo);
		draw::title_width = push_title;
	} else {
		draw::label("Освещение факела", 3);
		draw::label("Инструменты и оборудование", 5);
	}
}

static void right_panel() {
	draw::vertical(party_information);
	draw::propertybar();
	tabs_panel();
}

static creature* create_hero(bool interactive) {
	auto push_interactive = logs::interactive;
	logs::interactive = interactive;
	auto p = bsdata<creature>::add();
	p->generate();
	logs::interactive = push_interactive;
	return p;
}

static void generate_character() {
	logs::header = getnm("CharacterGeneration");
	logs::url = "meet";
	create_hero(false);
	create_hero(false);
	create_hero(true);
}

int main(int argc, char* argv[]) {
	initialize_png();
	answers::beforepaint = right_panel;
	draw::pstatus = statusinfo;
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}