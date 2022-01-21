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

void paint_items() {
}

bool view_character(const void* object) {
	return bsdata<creature>::have(draw::focus_object);
}

static widget right_pages[] = {
	{"CharacterAbility", 0, view_character},
	{}
};

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
	logs::interactive = true;
	create_hero(false);
	create_hero(true);
	create_hero(true);
}

int main(int argc, char* argv[]) {
	draw::protogonists = bsdata<varianti>::elements + Creature;
	draw::pstatus = statusinfo;
	draw::panel_pages = right_pages;
	quest::read("rules/Quest.txt");
	return draw::utg::run(generate_character, true);
}

int _stdcall WinMain(void* ci, void* pi, char* cmd, int sw) {
	return main(0, 0);
}