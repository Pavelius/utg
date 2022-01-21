#include "main.h"

static void getinformation(const char* id, stringbuilder& sb) {
	auto p = getdescription(id);
	if(!p)
		return;
	sb.add(p);
}

static void statusinfo(const void* object, stringbuilder& sb) {
	if(bsdata<creature>::source.have(object))
		((creature*)object)->getinfo(sb);
	else if(bsdata<abilityi>::source.have(object))
		getinformation(((abilityi*)object)->id, sb);
}

static void party_information() {
	for(auto i = 0; i < 4; i++) {
		auto& e = bsdata<creature>::elements[i];
		if(e)
			draw::avatar(i, &e, e.getavatar());
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

static void apply_test(void* object) {
}

static void generate_character() {
	logs::header = getnm("CharacterGeneration");
	logs::url = "meet";
	logs::interactive = true;
	create_hero(false);
	create_hero(true);
	create_hero(true);
}

int main(int argc, char* argv[]) {
	answers::beforepaint = right_panel;
	draw::pstatus = statusinfo;
	quest::read("rules/Quest.txt");
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}